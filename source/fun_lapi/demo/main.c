#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "stringbuf.h"
#include "event.h"
#include "net.h"
#include "task.h"
#include "tcpserver.h"
#include "http_test.h"

#if 0
void ignore_signal_handler(int sig) {}


static void exception_action(int signo, siginfo_t* info, void* ct)
{

}

void ignore_signals()
{
    struct sigaction sigact;
    sigemptyset (&sigact.sa_mask);
    memset(&sigact, 0, sizeof (struct sigaction));
    sigact.sa_flags = SA_ONESHOT | SA_SIGINFO;
    sigact.sa_sigaction = exception_action;
    sigaction(SIGINT, &sigact, NULL);
    sigaction(SIGQUIT, &sigact, NULL);
    sigaction(SIGILL, &sigact, NULL);
    sigaction(SIGTRAP, &sigact, NULL);
    sigaction(SIGABRT, &sigact, NULL);
    sigaction(SIGBUS, &sigact, NULL);
    sigaction(SIGFPE, &sigact, NULL);
    sigaction(SIGSEGV, &sigact, NULL);
    sigaction(SIGPWR, &sigact, NULL);
    sigaction(SIGSYS, &sigact, NULL);

    signal(SIGPIPE, ignore_signal_handler);
    signal(SIGTERM, ignore_signal_handler);
    signal(SIGHUP, ignore_signal_handler);

}

void tm_test(hTmEvent tm,void *priv)
{
    printf("jfkdsjfldsjfsl\n");

    timeout_event_settime(tm,1000);
}

void tm2_test(hTmEvent tm,void *priv)
{
    printf("#########2222\n");
    timeout_event_settime(tm,500);
}

void net_test(hFdEvent fd,void *priv)
{
    hSock tcp = (hSock)priv;
    int ret = fd_event_getevents(fd);
    if(ret != -1)
    {
        char buffer[10] = "";
        if(ret & EVENT_READ)
        {


        }
        if(ret & EVENT_WRITE)
        {
            sox_send(tcp,"hongtao",9);
        }
    }

}

void *event_loop(void *priv)
{
    hEpoll epoll = (hEpoll)priv;

    event_loop_run(epoll);
}

int main(int argc,char *argv[])
{
    strbuf_t sb;
    strbuf_init(&sb,1024);

    sb.append(&sb,"hongtao%d",1);

    sb.append(&sb,"hongtao%d",10);

    printf("%s\n",sb.buf);
    sb.print(&sb);

    strbuf_release(&sb);



    hEpoll epoll = event_loop_init(5);
    if(!epoll)
    {
        printf("create failed\n");
        return 0;
    }

    hTmEvent tm = timeout_event_new(1000);
    if(!tm)
    {
        printf("create tm failed\n");

        event_loop_release(epoll);
        return 0;
    }
    timeout_event_setfunc(tm,tm_test,tm);
    timeout_event_add(epoll,tm);

    hTmEvent tm2 = timeout_event_new(500);
    if(!tm2)
    {
        printf("create tm 2 failed\n");
        return 0;
    }

    timeout_event_setfunc(tm2,tm2_test,tm2);
    timeout_event_add(epoll,tm2);

    IPAddress addr =
    {
        6800,
        {"192.168.0.129"}
        //{"172.23.233.35"}
    };
    hSock tcp = sox_tcp(&addr,256);

    //sox_connect(tcp,&addr);
    sox_listen(tcp,10);



    int fd = sox_get_fd(tcp);
    hFdEvent fd_event = fd_event_new(fd,EVENT_READ |EVENT_WRITE);
    fd_event_setfunc(fd_event,net_test,tcp);
    fd_event_add(epoll,fd_event);

    lapi_thread_create(event_loop,epoll,512);




    return 0;
}
#endif

static hMutex g_mtx = NULL;
static int count = 0;

void have_conn(hSock sox, void *context)
{
    lapi_mutex_lock(g_mtx);
    count++;
    lapi_mutex_unlock(g_mtx);

    printf("connect add %d", count);
}

void have_close(hSock sox, void *context)
{
    lapi_mutex_lock(g_mtx);
    count--;
    lapi_mutex_unlock(g_mtx);

    printf("connect del %d", count);
}

void have_data(hSock sox, void *context)
{
    char buffer[20] = "";
    sock_recv(sox,buffer,sizeof(buffer));
    printf("buffer = %s\n",buffer);
}

void test_webs()
{
    g_mtx = lapi_mutex_create();

    TSPARAMS tsp;
    tsp.sox_bufsize = 64<<10;
    tsp.conn_num = 1000;
    tsp.thread_num = 50;
    tsp.cb_conn = have_conn;
    tsp.cb_close = have_close;
    tsp.cb_data = have_data;

    IPAddress local;
    strcpy(local.ip, "0.0.0.0");
    local.port = 4444;

    hTcpServer ts = ts_startup(&local, &tsp);
    if (!ts)
    {
        printf("ts_startup fail!");
    }
}

int main()
{
    //test_webs();
    http_test_get();
    while(1) ;
}
