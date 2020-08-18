#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <poll.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "net.h"

static void net_sock_set_flags(int sock, unsigned int type)
{
    if (!(type & NET_SOCK_NOCLOEXEC))
        fcntl(sock, F_SETFD, fcntl(sock, F_GETFD) | FD_CLOEXEC);

    if (type & NET_SOCK_NONBLOCK)
        fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK);
}


static int net_sock_connect(int type, struct sockaddr *sa, int sa_len, int family, int socktype, bool server)
{
    int sock;

    sock = socket(family, socktype, 0);
    if (sock < 0)
        return -1;

    net_sock_set_flags(sock, type);

    if (server) {
        const int one = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

        if (!bind(sock, sa, sa_len) &&
            (socktype != SOCK_STREAM || !listen(sock, SOMAXCONN)))
            return sock;
    } else {
        if (!connect(sock, sa, sa_len) || errno == EINPROGRESS)
            return sock;
    }

    close(sock);
    return -1;
}


static int net_sock_unix(int type, const char *host)
{
    struct sockaddr_un sun = {.sun_family = AF_UNIX};
    bool server = !!(type & NET_SOCK_SERVER);
    int socktype = ((type & 0xff) == NSOCK_TCP) ? SOCK_STREAM : SOCK_DGRAM;

    if (strlen(host) >= sizeof(sun.sun_path)) {

        errno = EINVAL;
        return -1;
    }
    strcpy(sun.sun_path, host);

    return net_sock_connect(type, (struct sockaddr*)&sun, sizeof(sun), AF_UNIX, socktype, server);
}


static int net_sock_inet_notimeout(int type, struct addrinfo *result, void *addr)
{
    struct addrinfo *rp;
    int socktype = ((type & 0xff) == NSOCK_TCP) ? SOCK_STREAM : SOCK_DGRAM;
    bool server = !!(type & NET_SOCK_SERVER);
    int sock;

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sock = net_sock_connect(type, rp->ai_addr, rp->ai_addrlen, rp->ai_family, socktype, server);
        if (sock >= 0) {
            if (addr)
                memcpy(addr, rp->ai_addr, rp->ai_addrlen);
            return sock;
        }
    }

    return -1;
}


static int poll_restart(struct pollfd *fds, int nfds, int timeout)
{
    struct timespec ts, cur;
    int msec = timeout % 1000;
    int ret;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    ts.tv_nsec += msec * 1000000;
    if (ts.tv_nsec > 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }
    ts.tv_sec += timeout / 1000;

    while (1) {
        ret = poll(fds, nfds, timeout);
        if (ret == EAGAIN)
            continue;

        if (ret != EINTR)
            return ret;

        clock_gettime(CLOCK_MONOTONIC, &cur);
        timeout = (ts.tv_sec - cur.tv_sec) * 1000;
        timeout += (ts.tv_nsec - cur.tv_nsec) / 1000000;
        if (timeout <= 0)
            return 0;
    }
}


int net_sock_inet_timeout(int type, const char *host, const char *service,
               void *addr, int timeout)
{
    int socktype = ((type & 0xff) == NSOCK_TCP) ? SOCK_STREAM : SOCK_DGRAM;
    bool server = !!(type & NET_SOCK_SERVER);
    struct addrinfo *result, *rp;
    struct addrinfo hints = {
        .ai_family = (type & NET_SOCK_IPV6ONLY) ? AF_INET6 :
            (type & NET_SOCK_IPV4ONLY) ? AF_INET : AF_UNSPEC,
        .ai_socktype = socktype,
        .ai_flags = AI_ADDRCONFIG
            | ((type & NET_SOCK_SERVER) ? AI_PASSIVE : 0)
            | ((type & NET_SOCK_NUMERIC) ? AI_NUMERICHOST : 0),
    };
    struct addrinfo *rp_v6 = NULL;
    struct addrinfo *rp_v4 = NULL;
    struct pollfd pfds[2] = {
        { .fd = -1, .events = POLLOUT },
        { .fd = -1, .events = POLLOUT },
    };
    int sock = -1;
    int i;

    if (getaddrinfo(host, service, &hints, &result))
        return -1;

    if (timeout <= 0 || server) {
        sock = net_sock_inet_notimeout(type, result, addr);
        goto free_addrinfo;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_family == AF_INET6 && !rp_v6)
            rp_v6 = rp;
        if (rp->ai_family == AF_INET && !rp_v4)
            rp_v4 = rp;
    }

    if (!rp_v6 && !rp_v4)
        goto out;

    if (rp_v6) {
        rp = rp_v6;
        pfds[0].fd = net_sock_connect(type | NET_SOCK_NONBLOCK, rp->ai_addr,
                       rp->ai_addrlen, rp->ai_family,
                       socktype, server);
        if (pfds[0].fd < 0) {
            rp_v6 = NULL;
            goto try_v4;
        }

        if (timeout > 300) {
            if (poll_restart(pfds, 1, 300) == 1) {
                rp = rp_v6;
                sock = pfds[0].fd;
                goto out;
            }
        }
        timeout -= 300;
    }

try_v4:
    if (rp_v4) {
        rp = rp_v4;
        pfds[1].fd = net_sock_connect(type | NET_SOCK_NONBLOCK, rp->ai_addr,
                         rp->ai_addrlen, rp->ai_family,
                         socktype, server);
        if (pfds[1].fd < 0) {
            rp_v4 = NULL;
            if (!rp_v6)
                goto out;
            goto wait;
        }
    }

wait:
    poll_restart(pfds + !rp_v6, !!rp_v6 + !!rp_v4, timeout);
    if (pfds[0].revents & POLLOUT) {
        rp = rp_v6;
        sock = pfds[0].fd;
        goto out;
    }

    if (pfds[1].revents & POLLOUT) {
        rp = rp_v4;
        sock = pfds[1].fd;
        goto out;
    }

out:
    for (i = 0; i < 2; i++) {
        int fd = pfds[i].fd;
        if (fd >= 0 && fd != sock)
            close(fd);
    }

    if (!(type & NET_SOCK_NONBLOCK))
        fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) & ~O_NONBLOCK);

    if (addr && sock >= 0)
        memcpy(addr, rp->ai_addr, rp->ai_addrlen);
free_addrinfo:
    freeaddrinfo(result);
    return sock;
}

const char *net_sock_port(int port)
{
    static char buffer[sizeof("65535\0")];

    if (port < 0 || port > 65535)
        return NULL;

    snprintf(buffer, sizeof(buffer), "%u", port);

    return buffer;
}


int net_sock(int type, const char *host, const char *service) {
    int sock;

    if (type & NET_SOCK_UNIX)
        sock = net_sock_unix(type, host);
    else
        sock = net_sock_inet(type, host, service, NULL);

    if (sock < 0)
        return -1;

    return sock;
}

int net_sock_wait_ready(int fd, int msecs) {
    struct pollfd fds[1];
    int res;

    fds[0].fd = fd;
    fds[0].events = POLLOUT;

    res = poll(fds, 1, msecs);
    if (res < 0) {
        return errno;
    } else if (res == 0) {
        return -ETIMEDOUT;
    } else {
        int err = 0;
        socklen_t optlen = sizeof(err);

        res = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &optlen);
        if (res)
            return errno;
        if (err)
            return err;
    }

    return 0;
}
