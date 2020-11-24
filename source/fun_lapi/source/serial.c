#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#ifdef __linux__
#include <termios.h>
#include <errno.h>
#endif
#include <unistd.h>


#include "task.h"
#include "list.h"

#include "serial.h"

/*
**  ************************************************************************
**                             宏定义
**  ************************************************************************
*/
#define TTY_OPEN_COUNT      2       //尝试打开AT命令对应ttyUSB设备文件的次数
#define TTY_WAIT_TIME       5       //打开AT命令对应ttyUSB设备文件失败时等待几秒再试

#define COMM_PURGE_TXABORT			0x0001	//禁止接收
#define COMM_PURGE_RXABORT			0x0002	//禁止接收
#define COMM_PURGE_TXCLEAR			0x0004	//清空发送缓存
#define COMM_PURGE_RXCLEAR			0x0008	//清空接收缓存

/*
**  ************************************************************************
**                             结构体定义
**  ************************************************************************
*/

/*
**  ************************************************************************
**                             全局变量定义
**  ************************************************************************
*/

/*
**  ************************************************************************
**                             函数申明
**  ************************************************************************
*/
/* $Function        :   purge_usb_com
==  ===============================================================
==  Description     :   开关ttyUSB接口的收发及缓存
==  Argument        :   [IN] tty_fd: 3G模块ttyUSB接口的文件描述符
==  Return          :   0: 成功  -1: 失败
==  Modification    :   2011/6/29    huang_zhendi    创建
==  ===============================================================
*/
int purge_usb_com(int tty_fd, int dw_flags)
{
    switch (dw_flags)
    {
        case COMM_PURGE_TXABORT:
            tcflow(tty_fd, TCOOFF);
            break;
        case COMM_PURGE_RXABORT:
            tcflow(tty_fd, TCIOFF);
            break;
        case COMM_PURGE_TXCLEAR:
            tcflush(tty_fd, TCOFLUSH);
            break;
        case COMM_PURGE_RXCLEAR:
            tcflush(tty_fd, TCIFLUSH);
            break;
        default:

            return -1;
    }

    return 0;
}

#define COM_BUFSIZE 1024
typedef struct
{
    int fd_tty;
    char tty_dev[128];
    int port;
    CommAttribute attr;

    CB_COM_RxDATA_FUNC cbRxData;
    void *ctx;

    char msgRecvBuf[COM_BUFSIZE];
    int  msgRecvOffset;		//当前已经接收的buffer长度
    int timeout;
    struct dl_list list;

}Comm_t;


static struct dl_list s_comms;
static hThread  thread = NULL;
static int first = 1;
static hMutex s_comm_mtx;


int serial_setAttr(hSerial h, const CommAttribute *attr)
{
    struct termios opt;
    Comm_t *comm = (Comm_t*)h;
    if(!comm || !attr)
        return -1;

    memcpy(&comm->attr,attr,sizeof(CommAttribute));

    memset(&opt, 0, sizeof(struct termios));
    tcgetattr(comm->fd_tty, &opt);
    cfmakeraw(&opt);			/* set raw mode	*/
    opt.c_iflag = IGNBRK;
    switch (attr->baudrate)
    {
        case 50:
            cfsetispeed(&opt, B50);
            cfsetospeed(&opt, B50);
            break;
        case 75:
            cfsetispeed(&opt, B75);
            cfsetospeed(&opt, B75);
            break;
        case 110:
            cfsetispeed(&opt, B110);
            cfsetospeed(&opt, B110);
            break;
        case 134:
            cfsetispeed(&opt, B134);
            cfsetospeed(&opt, B134);
            break;
        case 150:
            cfsetispeed(&opt, B150);
            cfsetospeed(&opt, B150);
            break;
        case 200:
            cfsetispeed(&opt, B200);
            cfsetospeed(&opt, B200);
            break;
        case 300:
            cfsetispeed(&opt, B300);
            cfsetospeed(&opt, B300);
            break;
        case 600:
            cfsetispeed(&opt, B600);
            cfsetospeed(&opt, B600);
            break;
        case 1200:
            cfsetispeed(&opt, B1200);
            cfsetospeed(&opt, B1200);
            break;
        case 1800:
            cfsetispeed(&opt, B1800);
            cfsetospeed(&opt, B1800);
            break;
        case 2400:
            cfsetispeed(&opt, B2400);
            cfsetospeed(&opt, B2400);
            break;
        case 4800:
            cfsetispeed(&opt, B4800);
            cfsetospeed(&opt, B4800);
            break;
        case 9600:
            cfsetispeed(&opt, B9600);
            cfsetospeed(&opt, B9600);
            break;
        case 19200:
            cfsetispeed(&opt, B19200);
            cfsetospeed(&opt, B19200);
            break;
        case 38400:
            cfsetispeed(&opt, B38400);
            cfsetospeed(&opt, B38400);
            break;
        case 57600:
            cfsetispeed(&opt, B57600);
            cfsetospeed(&opt, B57600);
            break;
        case 115200:
            cfsetispeed(&opt, B115200);
            cfsetospeed(&opt, B115200);
            break;
        case 230400:
            cfsetispeed(&opt, B230400);
            cfsetospeed(&opt, B230400);
            break;
        case 460800:
            cfsetispeed(&opt, B460800);
            cfsetospeed(&opt, B460800);
            break;
        case 500000:
            cfsetispeed(&opt, B500000);
            cfsetospeed(&opt, B500000);
            break;
        case 576000:
            cfsetispeed(&opt, B576000);
            cfsetospeed(&opt, B576000);
            break;
        case 921600:
            cfsetispeed(&opt, B921600);
            cfsetospeed(&opt, B921600);
            break;
        case 1000000:
            cfsetispeed(&opt, B1000000);
            cfsetospeed(&opt, B1000000);
            break;
        case 1152000:
            cfsetispeed(&opt, B1152000);
            cfsetospeed(&opt, B1152000);
            break;
        case 1500000:
            cfsetispeed(&opt, B1500000);
            cfsetospeed(&opt, B1500000);
            break;
        case 2000000:
            cfsetispeed(&opt, B2000000);
            cfsetospeed(&opt, B2000000);
            break;
        case 2500000:
            cfsetispeed(&opt, B2500000);
            cfsetospeed(&opt, B2500000);
            break;
        case 3000000:
            cfsetispeed(&opt, B3000000);
            cfsetospeed(&opt, B3000000);
            break;
        case 3500000:
            cfsetispeed(&opt, B3500000);
            cfsetospeed(&opt, B3500000);
            break;
        case 4000000:
            cfsetispeed(&opt, B4000000);
            cfsetospeed(&opt, B4000000);
            break;
        default:

            return -1;
    }


    switch (attr->parity)
    {
        case COMM_PARITY_NONE:		    /* none			*/
            opt.c_cflag &= ~PARENB;	/* disable parity	*/
            opt.c_iflag &= ~INPCK;	/* disable parity check	*/
            break;
        case COMM_PARITY_ODD:		/* odd			*/
            opt.c_cflag |= PARENB;	/* enable parity	*/
            opt.c_cflag |= PARODD;	/* odd			*/
            opt.c_iflag |= INPCK;	/* enable parity check	*/
            break;
        case COMM_PARITY_EVEN:		/* even			*/
            opt.c_cflag |= PARENB;	/* enable parity	*/
            opt.c_cflag &= ~PARODD;	/* even			*/
            opt.c_iflag |= INPCK;	/* enable parity check	*/
        default:

            return -1;
    }

    opt.c_cflag &= ~CSIZE;
    switch (attr->dataBits)
    {
        case 5:
            opt.c_cflag |= CS5;
            break;
        case 6:
            opt.c_cflag |= CS6;
            break;
        case 7:
            opt.c_cflag |= CS7;
            break;
        case 8:
            opt.c_cflag |= CS8;
            break;
        default:

            return -1;
    }

    opt.c_cflag &= ~CSTOPB;
    switch (attr->stopBits)
    {
        case COMM_STOPBIT_1:
            opt.c_cflag &= ~CSTOPB;
            break;
/*		case COMM_STOPBIT_1_5:
            break;
*/
        case COMM_STOPBITS_2:
            opt.c_cflag |= CSTOPB;
            break;
        default:

            return -1;
    }
    opt.c_cc[VTIME]	= 0;
    opt.c_cc[VMIN]	= 1;			/* block until data arrive */

    tcflush(comm->fd_tty, TCIOFLUSH);
    if (tcsetattr(comm->fd_tty, TCSANOW, &opt) < 0)
    {

        return -1;
    }

    //清空收发缓存，避免收到残留的数据
    purge_usb_com(comm->fd_tty, COMM_PURGE_TXCLEAR);
    purge_usb_com(comm->fd_tty, COMM_PURGE_RXCLEAR);

    return 0;
}


hSerial serial_open(const char* tty_dev)
{
    int i,fd_tty;
    Comm_t *comm = NULL;
    if(!tty_dev)
    {
        return NULL;
    }

    // 当模块在系统检测到后TTY_WAIT_TIME s内还打不开端口，需重启模块
    // wcdma模块有这现象,能检测到，但没有ttyUSB2
    for (i=0; i<TTY_OPEN_COUNT; i++)
    {
        fd_tty = open(tty_dev, O_RDWR|O_NOCTTY| O_NDELAY);
        if(fd_tty < 0)
        {
            lapi_sleep(TTY_WAIT_TIME*1000);  //等待TTY_WAIT_TIME秒
                continue;
        }

        break;
    }

    if (i == TTY_OPEN_COUNT)
    {
        /* error*/
        return NULL;
    }

     comm = (Comm_t *)calloc(1,sizeof(Comm_t));
    if(!comm)
    {
        close(fd_tty);

        return NULL;
    }
    comm->fd_tty = fd_tty;
    strncpy(comm->tty_dev,tty_dev,strlen(tty_dev)+1);
    dl_list_init(&comm->list);


    return (hSerial)comm;
}

int serial_close(hSerial h)
{
    lapi_mutex_lock(s_comm_mtx);
    Comm_t *comm = (Comm_t *)h;
    if(!comm )
    {
        lapi_mutex_unlock(s_comm_mtx);
        return -1;
    }

    close(comm->fd_tty);

    free(comm);
    comm = NULL;
    lapi_mutex_unlock(s_comm_mtx);

    return 0;
}

int serial_write(hSerial h, void *pdata, int len)
{
    Comm_t *comm = (Comm_t*)h;
    if(!comm || !pdata || (len == 0) )
        return -1;

    int cur = 0;
    /* the main string */
    while (cur < len) {
        int written;

        written = write (comm->fd_tty, pdata + cur, len - cur);
        if (written < 0)
        {
            /*被中断 或资源不足*/
            if(errno == EINTR ||
                errno == EAGAIN)
                continue;

            return -1;
        }

        cur += written;
    }

    return cur;
}

int serial_setRxDataCallBack(hSerial h, CB_COM_RxDATA_FUNC cbRxData, void *ctx)
{
    Comm_t *comm = (Comm_t *)h;
    if(!comm )
        return -1;

    comm->cbRxData = cbRxData;
    comm->ctx = ctx;

    return 0;

}

void * comm_recv_task(hThread h, void *p)
{
    int ret, len;
    fd_set	readfds;

    int timeout = 1000; // 1s


    while (lapi_thread_isrunning(h))
    {
        FD_ZERO(&readfds);
        Comm_t *comm = NULL;
        int maxfd = 0;
        lapi_mutex_lock(s_comm_mtx);
        dl_list_for_each(comm,&s_comms,Comm_t,list)
        {
            if(comm)
            {
                FD_SET(comm->fd_tty, &readfds);
                maxfd = MAX(maxfd, comm->fd_tty);
            }
        }
        lapi_mutex_unlock(s_comm_mtx);

        if(timeout)
        {
             struct timeval tm ={1,0};
            ret = select(maxfd + 1, &readfds, NULL, NULL, &tm);
        }
        else
        {
            ret = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        }

        if (ret < 0)
        {
            if (errno == EINTR)//避免因信号引起的select返回
            {
                continue;
            }

            break;
        }

        if (ret == 0) //超时返回
        {
            continue;
        }

        lapi_mutex_lock(s_comm_mtx);
        dl_list_for_each(comm,&s_comms,Comm_t,list)
        {

            if (comm  && FD_ISSET(comm->fd_tty, &readfds)) /* read data..*/
            {
                //printf("%s %d offset=%d \n",__FUNCTION__,__LINE__,comm->msgRecvOffset);
                memset(&comm->msgRecvBuf[comm->msgRecvOffset],0,COM_BUFSIZE - comm->msgRecvOffset);

                len = read(comm->fd_tty, &comm->msgRecvBuf[comm->msgRecvOffset], COM_BUFSIZE - comm->msgRecvOffset);

                if(len > 0)
                {
                    int rxLen = 0;
                    comm->msgRecvOffset += len;
                    //printf("%s %d len=%d offset=%d \n",__FUNCTION__,__LINE__,len,comm->msgRecvOffset);
                    if( comm->cbRxData)
                        rxLen = comm->cbRxData((hSerial)comm, comm->msgRecvBuf,comm->msgRecvOffset, comm->ctx);
                    //printf("%s %d hasProcLen=%d \n",__FUNCTION__,__LINE__,rxLen);
                    if(rxLen > 0)
                    {
                        memmove(comm->msgRecvBuf,&comm->msgRecvBuf[rxLen],comm->msgRecvOffset - rxLen);
                        comm->msgRecvOffset -= rxLen;
                    }
                }

            }
        }
        lapi_mutex_unlock(s_comm_mtx);


    }

    return NULL;


}

//启动comm异步监听
//time: 允许等待时间，毫秒单位，0: 永久等待
int serial_start(hSerial h, int time)
{
    Comm_t *comm = (Comm_t *)h;
    if(!comm )
        return -1;

    comm->timeout = time;

    if(first)
    {
        dl_list_init(&s_comms);
        s_comm_mtx = lapi_mutex_create();
    }

    lapi_mutex_lock(s_comm_mtx);
    dl_list_add_tail(&s_comms, &comm->list);
    lapi_mutex_unlock(s_comm_mtx);

    if(first)
    {
        first = 0;
        thread = lapi_thread_create(comm_recv_task,NULL,1<<10 << 10);
    }



    return 0;
}

//关闭comm异步监听
int serial_stop(hSerial h)
{
    lapi_mutex_lock(s_comm_mtx);
    Comm_t *comm = (Comm_t *)h;
    if(!comm )
    {
        lapi_mutex_unlock(s_comm_mtx);
        return -1;
    }

    dl_list_del(&comm->list);
    lapi_mutex_unlock(s_comm_mtx);

    return 0;
}


