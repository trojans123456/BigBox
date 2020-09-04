
#include "serial.h"

unsigned long get_termios_get_ioctl(void)
{
#ifdef HAVE_STRUCT_TERMIOS2
    return TCGETS2;
#else
    return TCGETS;
#endif
}

unsigned long get_termios_set_ioctl(void)
{
#ifdef HAVE_STRUCT_TERMIOS2
    return TCSETS2;
#else
    return TCSETS;
#endif
}

size_t get_termios_size(void)
{
#ifdef HAVE_STRUCT_TERMIOS2
    return sizeof(struct termios2);
#else
    return sizeof(struct termios);
#endif
}

int get_termios_speed(void *data)
{
#ifdef HAVE_STRUCT_TERMIOS2
    struct termios2 *term = (struct termios2 *) data;
#else
    struct termios *term = (struct termios *) data;
#endif
    if (term->c_ispeed != term->c_ospeed)
        return -1;
    else
        return term->c_ispeed;
}

void set_termios_speed(void *data, int speed)
{
#ifdef HAVE_STRUCT_TERMIOS2
    struct termios2 *term = (struct termios2 *) data;
#else
    struct termios *term = (struct termios *) data;
#endif
    term->c_cflag &= ~CBAUD;
    //term->c_cflag |= BOTHER;
    term->c_ispeed = term->c_ospeed = speed;
}

/** termiox 扩展终端  */
#if 0
size_t get_termiox_size(void)
{
    return sizeof(struct termiox);
}

int get_termiox_flow(void *data, int *rts, int *cts, int *dtr, int *dsr)
{
    struct termiox *termx = (struct termiox *) data;
    int flags = 0;

    *rts = (termx->x_cflag & RTSXOFF);
    *cts = (termx->x_cflag & CTSXON);
    *dtr = (termx->x_cflag & DTRXOFF);
    *dsr = (termx->x_cflag & DSRXON);

    return flags;
}

void set_termiox_flow(void *data, int rts, int cts, int dtr, int dsr)
{
    struct termiox *termx = (struct termiox *) data;

    termx->x_cflag &= ~(RTSXOFF | CTSXON | DTRXOFF | DSRXON);

    if (rts)
        termx->x_cflag |= RTSXOFF;
    if (cts)
        termx->x_cflag |= CTSXON;
    if (dtr)
        termx->x_cflag |= DTRXOFF;
    if (dsr)
        termx->x_cflag |= DSRXON;
}
#endif

static const struct std_baudrate std_baudrates[] = {
    BAUD(50), BAUD(75), BAUD(110), BAUD(134), BAUD(150), BAUD(200),
    BAUD(300), BAUD(600), BAUD(1200), BAUD(1800), BAUD(2400), BAUD(4800),
    BAUD(9600), BAUD(19200), BAUD(38400), BAUD(57600), BAUD(115200),
    BAUD(230400)
};

#define NUM_STD_BAUDRATES ARRAY_SIZE(std_baudrates)



#define CHECK_PORT() do { \
    if (!port) \
        return SERIAL_ERR_ARG; \
    if (!port->name) \
        return SERIAL_ERR_ARG; \
} while (0)

#define CHECK_PORT_HANDLE() do { \
    if (port->fd < 0) \
        return SERIAL_ERR_ARG; \
} while (0)

#define CHECK_OPEN_PORT() do { \
    CHECK_PORT(); \
    CHECK_PORT_HANDLE(); \
} while (0)


static enum serial_return get_baudrate(int fd, int *baudrate)
{
    void *data;

    if (!(data = malloc(get_termios_size())))
        return SERIAL_ERR_MEM;

    if (ioctl(fd, get_termios_get_ioctl(), data) < 0) {
        free(data);
        return SERIAL_ERR_FAIL;
    }

    *baudrate = get_termios_speed(data);

    free(data);

    return SERIAL_OK;
}

static enum serial_return set_baudrate(int fd, int baudrate)
{
    void *data;


    if (!(data = malloc(get_termios_size())))
        return SERIAL_ERR_MEM;

    if (ioctl(fd, get_termios_get_ioctl(), data) < 0) {
        free(data);
        return SERIAL_ERR_FAIL;
    }



    set_termios_speed(data, baudrate);

    if (ioctl(fd, get_termios_set_ioctl(), data) < 0) {
        free(data);
        return SERIAL_ERR_FAIL;
    }

    free(data);

    return SERIAL_OK;
}


#if 0
static enum serial_return get_flow(int fd, struct port_data *data)
{
    void *termx;


    if (!(termx = malloc(get_termiox_size())))
        return SERIAL_ERR_MEM;

    if (ioctl(fd, TCGETX, termx) < 0) {
        free(termx);
        return SERIAL_ERR_FAIL;
    }

    get_termiox_flow(termx, &data->rts_flow, &data->cts_flow,
            &data->dtr_flow, &data->dsr_flow);

    free(termx);

    return SERIAL_OK;
}

static enum serial_return set_flow(int fd, struct port_data *data)
{
    void *termx;


    if (!(termx = malloc(get_termiox_size())))
        return SERIAL_ERR_MEM;

    if (ioctl(fd, TCGETX, termx) < 0) {
        free(termx);
        return SERIAL_ERR_FAIL;
    }



    set_termiox_flow(termx, data->rts_flow, data->cts_flow,
            data->dtr_flow, data->dsr_flow);

    if (ioctl(fd, TCSETX, termx) < 0) {
        free(termx);
        return SERIAL_ERR_FAIL;
    }

    free(termx);

    return SERIAL_OK;
}
#endif

static enum serial_return get_config(struct serial_port *port, struct port_data *data,
    struct serial_port_config *config)
{
    unsigned int i;

    /**获取属性 */
    if (tcgetattr(port->fd, &data->term) < 0)
        return SERIAL_ERR_FAIL;

    /* TIOCMGET 获取RTS是否有信号 */
    if (ioctl(port->fd, TIOCMGET, &data->controlbits) < 0)
        return SERIAL_ERR_FAIL;


#if 0
    int ret = get_flow(port->fd, data);

    if (ret == SERIAL_ERR_FAIL && errno == EINVAL)
        data->termiox_supported = 0;
    else if (ret < 0)
        return ret;
    else
        data->termiox_supported = 1;
#else
    data->termiox_supported = 0;
#endif
    for (i = 0; i < NUM_STD_BAUDRATES; i++) {
        if (cfgetispeed(&data->term) == std_baudrates[i].index) {
            config->baudrate = std_baudrates[i].value;
            break;
        }
    }

    if (i == NUM_STD_BAUDRATES) {
        get_baudrate(port->fd, &config->baudrate);
    }

    switch (data->term.c_cflag & CSIZE) {
    case CS8:
        config->bits = 8;
        break;
    case CS7:
        config->bits = 7;
        break;
    case CS6:
        config->bits = 6;
        break;
    case CS5:
        config->bits = 5;
        break;
    default:
        config->bits = -1;
    }

    if (!(data->term.c_cflag & PARENB) && (data->term.c_iflag & IGNPAR))
        config->parity = SERIAL_PARITY_NONE;
    else if (!(data->term.c_cflag & PARENB) || (data->term.c_iflag & IGNPAR))
        config->parity = -1;
    else if (data->term.c_cflag & CMSPAR)
        config->parity = (data->term.c_cflag & PARODD) ? SERIAL_PARITY_MARK : SERIAL_PARITY_SPACE;
    else
        config->parity = (data->term.c_cflag & PARODD) ? SERIAL_PARITY_ODD : SERIAL_PARITY_EVEN;

    config->stopbits = (data->term.c_cflag & CSTOPB) ? 2 : 1;

    if (data->term.c_cflag & CRTSCTS) {
        config->rts = SERIAL_RTS_FLOW_CONTROL;
        config->cts = SERIAL_CTS_FLOW_CONTROL;
    } else {
        if (data->termiox_supported && data->rts_flow)
            config->rts = SERIAL_RTS_FLOW_CONTROL;
        else
            config->rts = (data->controlbits & TIOCM_RTS) ? SERIAL_RTS_ON : SERIAL_RTS_OFF;

        config->cts = (data->termiox_supported && data->cts_flow) ?
            SERIAL_CTS_FLOW_CONTROL : SERIAL_CTS_IGNORE;
    }

    if (data->termiox_supported && data->dtr_flow)
        config->dtr = SERIAL_DTR_FLOW_CONTROL;
    else
        config->dtr = (data->controlbits & TIOCM_DTR) ? SERIAL_DTR_ON : SERIAL_DTR_OFF;

    config->dsr = (data->termiox_supported && data->dsr_flow) ?
        SERIAL_DSR_FLOW_CONTROL : SERIAL_DSR_IGNORE;

    if (data->term.c_iflag & IXOFF) {
        if (data->term.c_iflag & IXON)
            config->xon_xoff = SERIAL_XONXOFF_INOUT;
        else
            config->xon_xoff = SERIAL_XONXOFF_IN;
    } else {
        if (data->term.c_iflag & IXON)
            config->xon_xoff = SERIAL_XONXOFF_OUT;
        else
            config->xon_xoff = SERIAL_XONXOFF_DISABLED;
    }


   return SERIAL_OK;
}

static enum serial_return set_config(struct serial_port *port, struct port_data *data,
    const struct serial_port_config *config)
{
    unsigned int i;
    int baud_nonstd = 0;
    int controlbits;

    if (config->baudrate >= 0) {
        for (i = 0; i < NUM_STD_BAUDRATES; i++) {
            if (config->baudrate == std_baudrates[i].value) {
                if (cfsetospeed(&data->term, std_baudrates[i].index) < 0)
                    return SERIAL_ERR_FAIL;

                if (cfsetispeed(&data->term, std_baudrates[i].index) < 0)
                    return SERIAL_ERR_FAIL;
                break;
            }
        }

        /* Non-standard baud rate */
        if (i == NUM_STD_BAUDRATES) {
            baud_nonstd = 1;
        }
    }

    if (config->bits >= 0) {
        data->term.c_cflag &= ~CSIZE;
        switch (config->bits) {
        case 8:
            data->term.c_cflag |= CS8;
            break;
        case 7:
            data->term.c_cflag |= CS7;
            break;
        case 6:
            data->term.c_cflag |= CS6;
            break;
        case 5:
            data->term.c_cflag |= CS5;
            break;
        default:
            return SERIAL_ERR_ARG;
        }
    }

    if (config->parity >= 0) {
        data->term.c_iflag &= ~IGNPAR;
        data->term.c_cflag &= ~(PARENB | PARODD);

        data->term.c_cflag &= ~CMSPAR;

        switch (config->parity) {
        case SERIAL_PARITY_NONE:
            data->term.c_iflag |= IGNPAR;
            break;
        case SERIAL_PARITY_EVEN:
            data->term.c_cflag |= PARENB;
            break;
        case SERIAL_PARITY_ODD:
            data->term.c_cflag |= PARENB | PARODD;
            break;
        case SERIAL_PARITY_MARK:
            data->term.c_cflag |= PARENB | PARODD;
            data->term.c_cflag |= CMSPAR;
            break;
        case SERIAL_PARITY_SPACE:
            data->term.c_cflag |= PARENB;
            data->term.c_cflag |= CMSPAR;
            break;


        default:
           return SERIAL_ERR_ARG;
        }
    }

    if (config->stopbits >= 0) {
        data->term.c_cflag &= ~CSTOPB;
        switch (config->stopbits) {
        case 1:
            data->term.c_cflag &= ~CSTOPB;
            break;
        case 2:
            data->term.c_cflag |= CSTOPB;
            break;
        default:
            return SERIAL_ERR_ARG;
        }
    }

    if (config->rts >= 0 || config->cts >= 0) {
        if (data->termiox_supported) {
            data->rts_flow = data->cts_flow = 0;
            switch (config->rts) {
            case SERIAL_RTS_OFF:
            case SERIAL_RTS_ON:
                controlbits = TIOCM_RTS;
                if (ioctl(port->fd, config->rts == SERIAL_RTS_ON ? TIOCMBIS : TIOCMBIC, &controlbits) < 0)
                    return SERIAL_ERR_FAIL;
                break;
            case SERIAL_RTS_FLOW_CONTROL:
                data->rts_flow = 1;
                break;
            default:
                break;
            }
            if (config->cts == SERIAL_CTS_FLOW_CONTROL)
                data->cts_flow = 1;

            if (data->rts_flow && data->cts_flow)
                data->term.c_iflag |= CRTSCTS;
            else
                data->term.c_iflag &= ~CRTSCTS;
        } else {
            /* Asymmetric use of RTS/CTS not supported. */
            if (data->term.c_iflag & CRTSCTS) {
                /* Flow control can only be disabled for both RTS & CTS together. */
                if (config->rts >= 0 && config->rts != SERIAL_RTS_FLOW_CONTROL) {
                    if (config->cts != SERIAL_CTS_IGNORE)
                        return SERIAL_ERR_SUPP;
                }
                if (config->cts >= 0 && config->cts != SERIAL_CTS_FLOW_CONTROL) {
                    if (config->rts <= 0 || config->rts == SERIAL_RTS_FLOW_CONTROL)
                        return SERIAL_ERR_SUPP;
                }
            } else {
                /* Flow control can only be enabled for both RTS & CTS together. */
                if (((config->rts == SERIAL_RTS_FLOW_CONTROL) && (config->cts != SERIAL_CTS_FLOW_CONTROL)) ||
                    ((config->cts == SERIAL_CTS_FLOW_CONTROL) && (config->rts != SERIAL_RTS_FLOW_CONTROL)))
                    return SERIAL_ERR_SUPP;
            }

            if (config->rts >= 0) {
                if (config->rts == SERIAL_RTS_FLOW_CONTROL) {
                    data->term.c_iflag |= CRTSCTS;
                } else {
                    controlbits = TIOCM_RTS;
                    if (ioctl(port->fd, config->rts == SERIAL_RTS_ON ? TIOCMBIS : TIOCMBIC,
                            &controlbits) < 0)
                        return SERIAL_ERR_FAIL;
                }
            }
        }
    }

    if (config->dtr >= 0 || config->dsr >= 0) {
        if (data->termiox_supported) {
            data->dtr_flow = data->dsr_flow = 0;
            switch (config->dtr) {
            case SERIAL_DTR_OFF:
            case SERIAL_DTR_ON:
                controlbits = TIOCM_DTR;
                if (ioctl(port->fd, config->dtr == SERIAL_DTR_ON ? TIOCMBIS : TIOCMBIC, &controlbits) < 0)
                    return SERIAL_ERR_FAIL;
                break;
            case SERIAL_DTR_FLOW_CONTROL:
                data->dtr_flow = 1;
                break;
            default:
                break;
            }
            if (config->dsr == SERIAL_DSR_FLOW_CONTROL)
                data->dsr_flow = 1;
        } else {
            /* DTR/DSR flow control not supported. */
            if (config->dtr == SERIAL_DTR_FLOW_CONTROL || config->dsr == SERIAL_DSR_FLOW_CONTROL)
                return SERIAL_ERR_SUPP;

            if (config->dtr >= 0) {
                controlbits = TIOCM_DTR;
                if (ioctl(port->fd, config->dtr == SERIAL_DTR_ON ? TIOCMBIS : TIOCMBIC,
                        &controlbits) < 0)
                    return SERIAL_ERR_FAIL;
            }
        }
    }

    if (config->xon_xoff >= 0) {
        data->term.c_iflag &= ~(IXON | IXOFF | IXANY);
        switch (config->xon_xoff) {
        case SERIAL_XONXOFF_DISABLED:
            break;
        case SERIAL_XONXOFF_IN:
            data->term.c_iflag |= IXOFF;
            break;
        case SERIAL_XONXOFF_OUT:
            data->term.c_iflag |= IXON | IXANY;
            break;
        case SERIAL_XONXOFF_INOUT:
            data->term.c_iflag |= IXON | IXOFF | IXANY;
            break;
        default:
            return SERIAL_ERR_ARG;
        }
    }

    if (tcsetattr(port->fd, TCSANOW, &data->term) < 0)
        return SERIAL_ERR_FAIL;

    if (baud_nonstd)
        set_baudrate(port->fd, config->baudrate);
#if 0
    if (data->termiox_supported)
        set_flow(port->fd, data);
#endif
    return SERIAL_OK;

}

enum serial_return serial_open(struct serial_port *port, enum serial_mode flags)
{
    struct port_data data;
    struct serial_port_config config;
    enum serial_return ret;


    CHECK_PORT();

    if (flags > SERIAL_MODE_READ_WRITE)
        return SERIAL_ERR_ARG;



    int flags_local = O_NONBLOCK | O_NOCTTY;

    /* Map 'flags' to the OS-specific settings. */
    if ((flags & SERIAL_MODE_READ_WRITE) == SERIAL_MODE_READ_WRITE)
        flags_local |= O_RDWR;
    else if (flags & SERIAL_MODE_READ)
        flags_local |= O_RDONLY;
    else if (flags & SERIAL_MODE_WRITE)
        flags_local |= O_WRONLY;

    if ((port->fd = open(port->name, flags_local)) < 0)
        return SERIAL_ERR_FAIL;

    /*通过data的term字段获取当前串口的配置存放到config中*/
    ret = get_config(port, &data, &config);

    if (ret < 0) {
        serial_close(port);
        return SERIAL_ERR_FAIL;
    }

    /* Set sane port settings. */
    /* Turn off all fancy termios tricks, give us a raw channel. */
    data.term.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IMAXBEL);

    data.term.c_iflag &= ~IUCLC;

    data.term.c_oflag &= ~(OPOST | ONLCR | OCRNL | ONOCR | ONLRET);

    data.term.c_oflag &= ~OLCUC;

    data.term.c_oflag &= ~NLDLY;

    data.term.c_oflag &= ~CRDLY;

    data.term.c_oflag &= ~TABDLY;

    data.term.c_oflag &= ~BSDLY;

    data.term.c_oflag &= ~VTDLY;

    data.term.c_oflag &= ~FFDLY;

    data.term.c_oflag &= ~OFILL;

    data.term.c_lflag &= ~(ISIG | ICANON | ECHO | IEXTEN);
    data.term.c_cc[VMIN] = 0;
    data.term.c_cc[VTIME] = 0;

    /* Ignore modem status lines; enable receiver; leave control lines alone on close. */
    data.term.c_cflag |= (CLOCAL | CREAD | HUPCL);


    ret = set_config(port, &data, &config);

    if (ret < 0) {
        serial_close(port);
        return ret;
    }

    return SERIAL_OK;
}

enum serial_return serial_close(struct serial_port *port)
{

    /* Returns 0 upon success, -1 upon failure. */
    if (close(port->fd) == -1)
        return SERIAL_ERR_FAIL;
    port->fd = -1;

    return SERIAL_OK;
}

enum serial_return serial_flush(struct serial_port *port, enum serial_buffer buffers)
{
    CHECK_OPEN_PORT();

    if (buffers > SERIAL_BUF_BOTH)
        return SERIAL_ERR_ARG;

    const char *buffer_names[] = {"no", "input", "output", "both"};


    int flags = 0;
    if (buffers == SERIAL_BUF_BOTH)
        flags = TCIOFLUSH;
    else if (buffers == SERIAL_BUF_INPUT)
        flags = TCIFLUSH;
    else if (buffers == SERIAL_BUF_OUTPUT)
        flags = TCOFLUSH;

    /* Returns 0 upon success, -1 upon failure. */
    if (tcflush(port->fd, flags) < 0)
        return SERIAL_ERR_FAIL;

    return SERIAL_OK;
}

enum serial_return serial_drain(struct serial_port *port)
{
    CHECK_OPEN_PORT();

    int result;
    while (1) {

        result = tcdrain(port->fd);

        if (result < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                return SERIAL_ERR_FAIL;
            }
        } else {
            return SERIAL_OK;
        }
    }

}

enum serial_return serial_blocking_write(struct serial_port *port, const void *buf,
                                        size_t count, unsigned int timeout_ms)
{
    CHECK_OPEN_PORT();

    if (!buf || count == 0)
        return SERIAL_ERR_ARG;


    size_t bytes_written = 0;
    unsigned char *ptr = (unsigned char *) buf;
    struct timeval start, delta, now, end = {0, 0};
    int started = 0;
    fd_set fds;
    int result;

    if (timeout_ms) {
        /* Get time at start of operation. */
        gettimeofday(&start, NULL);
        /* Define duration of timeout. */
        delta.tv_sec = timeout_ms / 1000;
        delta.tv_usec = (timeout_ms % 1000) * 1000;
        /* Calculate time at which we should give up. */
        timeradd(&start, &delta, &end);
    }

    FD_ZERO(&fds);
    FD_SET(port->fd, &fds);

    /* Loop until we have written the requested number of bytes. */
    while (bytes_written < count) {
        /*
         * Check timeout only if we have run select() at least once,
         * to avoid any issues if a short timeout is reached before
         * select() is even run.
         */
        if (timeout_ms && started) {
            gettimeofday(&now, NULL);
            if (timercmp(&now, &end, >))
                /* Timeout has expired. */
                break;
            timersub(&end, &now, &delta);
        }
        result = select(port->fd + 1, NULL, &fds, NULL, timeout_ms ? &delta : NULL);
        started = 1;
        if (result < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                return SERIAL_ERR_FAIL;
            }
        } else if (result == 0) {
            /* Timeout has expired. */
            break;
        }

        /* Do write. */
        result = write(port->fd, ptr, count - bytes_written);

        if (result < 0) {
            if (errno == EAGAIN)
                /* This shouldn't happen because we did a select() first, but handle anyway. */
                continue;
            else
                /* This is an actual failure. */
                return SERIAL_ERR_FAIL;
        }

        bytes_written += result;
        ptr += result;
    }

    return bytes_written;

}

enum serial_return serial_nonblocking_write(struct serial_port *port,
                                           const void *buf, size_t count)
{

    CHECK_OPEN_PORT();

    if (!buf || count == 0)
        return SERIAL_ERR_ARG;


    /* Returns the number of bytes written, or -1 upon failure. */
    ssize_t written = write(port->fd, buf, count);

    if (written < 0)
        return SERIAL_ERR_FAIL;
    else
        return (written);
}


enum serial_return serial_blocking_read(struct serial_port *port, void *buf,
                                       size_t count, unsigned int timeout_ms)
{
    CHECK_OPEN_PORT();

    if (!buf || count == 0)
        return SERIAL_ERR_ARG;


    size_t bytes_read = 0;
    unsigned char *ptr = (unsigned char *) buf;
    struct timeval start, delta, now, end = {0, 0};
    int started = 0;
    fd_set fds;
    int result;

    if (timeout_ms) {
        /* Get time at start of operation. */
        gettimeofday(&start, NULL);
        /* Define duration of timeout. */
        delta.tv_sec = timeout_ms / 1000;
        delta.tv_usec = (timeout_ms % 1000) * 1000;
        /* Calculate time at which we should give up. */
        timeradd(&start, &delta, &end);
    }

    FD_ZERO(&fds);
    FD_SET(port->fd, &fds);

    /* Loop until we have the requested number of bytes. */
    while (bytes_read < count) {
        /*
         * Check timeout only if we have run select() at least once,
         * to avoid any issues if a short timeout is reached before
         * select() is even run.
         */
        if (timeout_ms && started) {
            gettimeofday(&now, NULL);
            if (timercmp(&now, &end, >))
                /* Timeout has expired. */
                break;
            timersub(&end, &now, &delta);
        }
        result = select(port->fd + 1, &fds, NULL, NULL, timeout_ms ? &delta : NULL);
        started = 1;
        if (result < 0) {
            if (errno == EINTR) {

                continue;
            } else {
                return SERIAL_ERR_FAIL;
            }
        } else if (result == 0) {
            /* Timeout has expired. */
            break;
        }

        /* Do read. */
        result = read(port->fd, ptr, count - bytes_read);

        if (result < 0) {
            if (errno == EAGAIN)
                /*
                 * This shouldn't happen because we did a
                 * select() first, but handle anyway.
                 */
                continue;
            else
                /* This is an actual failure. */
                return SERIAL_ERR_FAIL;
        }

        bytes_read += result;
        ptr += result;
    }



    return (bytes_read);

}

enum serial_return serial_blocking_read_next(struct serial_port *port, void *buf,
                                            size_t count, unsigned int timeout_ms)
{
    CHECK_OPEN_PORT();

    if (!buf || count == 0)
        return SERIAL_ERR_ARG;


    size_t bytes_read = 0;
    struct timeval start, delta, now, end = {0, 0};
    int started = 0;
    fd_set fds;
    int result;

    if (timeout_ms) {
        /* Get time at start of operation. */
        gettimeofday(&start, NULL);
        /* Define duration of timeout. */
        delta.tv_sec = timeout_ms / 1000;
        delta.tv_usec = (timeout_ms % 1000) * 1000;
        /* Calculate time at which we should give up. */
        timeradd(&start, &delta, &end);
    }

    FD_ZERO(&fds);
    FD_SET(port->fd, &fds);

    /* Loop until we have at least one byte, or timeout is reached. */
    while (bytes_read == 0) {
        /*
         * Check timeout only if we have run select() at least once,
         * to avoid any issues if a short timeout is reached before
         * select() is even run.
         */
        if (timeout_ms && started) {
            gettimeofday(&now, NULL);
            if (timercmp(&now, &end, >))
                /* Timeout has expired. */
                break;
            timersub(&end, &now, &delta);
        }
        result = select(port->fd + 1, &fds, NULL, NULL, timeout_ms ? &delta : NULL);
        started = 1;
        if (result < 0) {
            if (errno == EINTR) {

                continue;
            } else {
                return SERIAL_ERR_FAIL;
            }
        } else if (result == 0) {
            /* Timeout has expired. */
            break;
        }

        /* Do read. */
        result = read(port->fd, buf, count);

        if (result < 0) {
            if (errno == EAGAIN)
                /* This shouldn't happen because we did a select() first, but handle anyway. */
                continue;
            else
                /* This is an actual failure. */
               return SERIAL_ERR_FAIL;
        }

        bytes_read = result;
    }


    return (bytes_read);

}

enum serial_return serial_nonblocking_read(struct serial_port *port, void *buf,
                                          size_t count)
{
    CHECK_OPEN_PORT();

    if (!buf)
        return SERIAL_ERR_ARG;


    ssize_t bytes_read;

    /* Returns the number of bytes read, or -1 upon failure. */
    if ((bytes_read = read(port->fd, buf, count)) < 0) {
        if (errno == EAGAIN)
            /* No bytes available. */
            bytes_read = 0;
        else
            /* This is an actual failure. */
            return SERIAL_ERR_FAIL;
    }
    return (bytes_read);

}

enum serial_return serial_input_waiting(struct serial_port *port)
{
    CHECK_OPEN_PORT();

    int bytes_waiting;
    if (ioctl(port->fd, TIOCINQ, &bytes_waiting) < 0)
        return SERIAL_ERR_FAIL;

    return (bytes_waiting);

}

enum serial_return serial_output_waiting(struct serial_port *port)
{
    CHECK_OPEN_PORT();

    int bytes_waiting;
    if (ioctl(port->fd, TIOCOUTQ, &bytes_waiting) < 0)
        return SERIAL_ERR_FAIL;

    return (bytes_waiting);
}

enum serial_return serial_new_event_set(struct serial_event_set **result_ptr)
{
    struct serial_event_set *result;



    if (!result_ptr)
        return SERIAL_ERR_ARG;

    *result_ptr = NULL;

    if (!(result = malloc(sizeof(struct serial_event_set))))
        return SERIAL_ERR_MEM;

    memset(result, 0, sizeof(struct serial_event_set));

    *result_ptr = result;

   return SERIAL_OK;
}

static enum serial_return add_handle(struct serial_event_set *event_set,
        event_handle handle, enum serial_event mask)
{
    void *new_handles;
    enum serial_event *new_masks;


    if (!(new_handles = realloc(event_set->handles,
            sizeof(event_handle) * (event_set->count + 1))))
        return SERIAL_ERR_MEM;

    event_set->handles = new_handles;

    if (!(new_masks = realloc(event_set->masks,
            sizeof(enum serial_event) * (event_set->count + 1))))
        return SERIAL_ERR_MEM;

    event_set->masks = new_masks;

    ((event_handle *) event_set->handles)[event_set->count] = handle;
    event_set->masks[event_set->count] = mask;

    event_set->count++;

    return SERIAL_OK;
}

enum serial_return serial_add_port_events(struct serial_event_set *event_set,
    const struct serial_port *port, enum serial_event mask)
{


    if (!event_set)
        return SERIAL_ERR_ARG;

    if (!port)
        return SERIAL_ERR_ARG;

    if (mask > (SERIAL_EVENT_RX_READY | SERIAL_EVENT_TX_READY | SERIAL_EVENT_ERROR))
        return SERIAL_ERR_ARG;

    if (!mask)
        return SERIAL_OK;


    add_handle(event_set, port->fd, mask);


    return SERIAL_OK;
}

void serial_free_event_set(struct serial_event_set *event_set)
{


    if (!event_set) {

        return ;
    }



    if (event_set->handles)
        free(event_set->handles);
    if (event_set->masks)
        free(event_set->masks);

    free(event_set);

}

enum serial_return serial_wait(struct serial_event_set *event_set,
                              unsigned int timeout_ms)
{


    if (!event_set)
        return SERIAL_ERR_ARG;


    struct timeval start, delta, now, end = {0, 0};
    const struct timeval max_delta = {
        (INT_MAX / 1000), (INT_MAX % 1000) * 1000};
    int started = 0, timeout_overflow = 0;
    int result, timeout_remaining_ms;
    struct pollfd *pollfds;
    unsigned int i;

    if (!(pollfds = malloc(sizeof(struct pollfd) * event_set->count)))
        return SERIAL_ERR_MEM;

    for (i = 0; i < event_set->count; i++) {
        pollfds[i].fd = ((int *) event_set->handles)[i];
        pollfds[i].events = 0;
        pollfds[i].revents = 0;
        if (event_set->masks[i] & SERIAL_EVENT_RX_READY)
            pollfds[i].events |= POLLIN;
        if (event_set->masks[i] & SERIAL_EVENT_TX_READY)
            pollfds[i].events |= POLLOUT;
        if (event_set->masks[i] & SERIAL_EVENT_ERROR)
            pollfds[i].events |= POLLERR;
    }

    if (timeout_ms) {
        /* Get time at start of operation. */
        gettimeofday(&start, NULL);
        /* Define duration of timeout. */
        delta.tv_sec = timeout_ms / 1000;
        delta.tv_usec = (timeout_ms % 1000) * 1000;
        /* Calculate time at which we should give up. */
        timeradd(&start, &delta, &end);
    }

    /* Loop until an event occurs. */
    while (1) {
        /*
         * Check timeout only if we have run poll() at least once,
         * to avoid any issues if a short timeout is reached before
         * poll() is even run.
         */
        if (!timeout_ms) {
            timeout_remaining_ms = -1;
        } else if (!started) {
            timeout_overflow = (timeout_ms > INT_MAX);
            timeout_remaining_ms = timeout_overflow ? INT_MAX : timeout_ms;
        } else {
            gettimeofday(&now, NULL);
            if (timercmp(&now, &end, >)) {

                break;
            }
            timersub(&end, &now, &delta);
            if ((timeout_overflow = timercmp(&delta, &max_delta, >)))
                delta = max_delta;
            timeout_remaining_ms = delta.tv_sec * 1000 + delta.tv_usec / 1000;
        }

        result = poll(pollfds, event_set->count, timeout_remaining_ms);
        started = 1;

        if (result < 0) {
            if (errno == EINTR) {

                continue;
            } else {
                free(pollfds);
                return SERIAL_ERR_FAIL;
            }
        } else if (result == 0) {

            if (!timeout_overflow)
                break;
        } else {

            break;
        }
    }

    free(pollfds);

    return SERIAL_OK;
}





enum serial_return serial_get_config(struct serial_port *port,
                                    struct serial_port_config *config)
{
    struct port_data data;


    CHECK_OPEN_PORT();

    if (!config)
        return SERIAL_ERR_ARG;

    get_config(port, &data, config);

    return SERIAL_OK;
}

enum serial_return serial_set_config(struct serial_port *port,
                                    const struct serial_port_config *config)
{
    struct port_data data;
    struct serial_port_config prev_config;



    CHECK_OPEN_PORT();

    if (!config)
        return SERIAL_ERR_ARG;

    get_config(port, &data, &prev_config);
    set_config(port, &data, config);

    return SERIAL_OK;
}

#define CREATE_ACCESSORS(x, type) \
enum serial_return serial_set_##x(struct serial_port *port, type x) { \
    struct port_data data; \
    struct serial_port_config config; \
    CHECK_OPEN_PORT(); \
    get_config(port, &data, &config); \
    config.x = x; \
    set_config(port, &data, &config); \
    return SERIAL_OK; \
} \
enum serial_return serial_get_config_##x(const struct serial_port_config *config, \
                                        type *x) { \
    if (!x) \
        return SERIAL_ERR_ARG; \
    if (!config) \
        return SERIAL_ERR_ARG; \
    *x = config->x; \
    return SERIAL_OK; \
} \
enum serial_return serial_set_config_##x(struct serial_port_config *config, \
                                        type x) { \
    if (!config) \
        return SERIAL_ERR_ARG; \
    config->x = x; \
    return SERIAL_OK; \
}

CREATE_ACCESSORS(baudrate, int)
CREATE_ACCESSORS(bits, int)
CREATE_ACCESSORS(parity, enum serial_parity)
CREATE_ACCESSORS(stopbits, int)
CREATE_ACCESSORS(rts, enum serial_rts)
CREATE_ACCESSORS(cts, enum serial_cts)
CREATE_ACCESSORS(dtr, enum serial_dtr)
CREATE_ACCESSORS(dsr, enum serial_dsr)
CREATE_ACCESSORS(xon_xoff, enum serial_xonxoff)

enum serial_return serial_set_config_flowcontrol(struct serial_port_config *config,
                                                enum serial_flowcontrol flowcontrol)
{
    if (!config)
        return SERIAL_ERR_ARG;

    if (flowcontrol > SERIAL_FLOWCONTROL_DTRDSR)
        return SERIAL_ERR_ARG;

    if (flowcontrol == SERIAL_FLOWCONTROL_XONXOFF)
        config->xon_xoff = SERIAL_XONXOFF_INOUT;
    else
        config->xon_xoff = SERIAL_XONXOFF_DISABLED;

    if (flowcontrol == SERIAL_FLOWCONTROL_RTSCTS) {
        config->rts = SERIAL_RTS_FLOW_CONTROL;
        config->cts = SERIAL_CTS_FLOW_CONTROL;
    } else {
        if (config->rts == SERIAL_RTS_FLOW_CONTROL)
            config->rts = SERIAL_RTS_ON;
        config->cts = SERIAL_CTS_IGNORE;
    }

    if (flowcontrol == SERIAL_FLOWCONTROL_DTRDSR) {
        config->dtr = SERIAL_DTR_FLOW_CONTROL;
        config->dsr = SERIAL_DSR_FLOW_CONTROL;
    } else {
        if (config->dtr == SERIAL_DTR_FLOW_CONTROL)
            config->dtr = SERIAL_DTR_ON;
        config->dsr = SERIAL_DSR_IGNORE;
    }

    return SERIAL_OK;
}

enum serial_return serial_set_flowcontrol(struct serial_port *port,
                                         enum serial_flowcontrol flowcontrol)
{
    struct port_data data;
    struct serial_port_config config;



    CHECK_OPEN_PORT();

    get_config(port, &data, &config);

    serial_set_config_flowcontrol(&config, flowcontrol);

    set_config(port, &data, &config);

    return SERIAL_OK;
}

enum serial_return serial_get_signals(struct serial_port *port,enum serial_signal *signals)
{


    CHECK_OPEN_PORT();

    if (!signals)
        return SERIAL_ERR_ARG;

    *signals = 0;

    int bits;
    if (ioctl(port->fd, TIOCMGET, &bits) < 0)
        return SERIAL_ERR_FAIL;
    if (bits & TIOCM_CTS)
        *signals |= SERIAL_SIG_CTS;
    if (bits & TIOCM_DSR)
        *signals |= SERIAL_SIG_DSR;
    if (bits & TIOCM_CAR)
        *signals |= SERIAL_SIG_DCD;
    if (bits & TIOCM_RNG)
        *signals |= SERIAL_SIG_RI;

    return SERIAL_OK;
}

enum serial_return serial_start_break(struct serial_port *port)
{


    CHECK_OPEN_PORT();

    if (ioctl(port->fd, TIOCSBRK, 1) < 0)
        return SERIAL_ERR_FAIL;


   return SERIAL_OK;
}

enum serial_return serial_end_break(struct serial_port *port)
{


    CHECK_OPEN_PORT();

    if (ioctl(port->fd, TIOCCBRK, 1) < 0)
        return SERIAL_ERR_FAIL;


    return SERIAL_OK;
}


/** @} */
