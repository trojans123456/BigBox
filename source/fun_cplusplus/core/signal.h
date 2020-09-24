#ifndef __SIGNAL_H
#define __SIGNAL_H

#include <signal.h>

namespace ns_core {

class signal
{
public:
    typedef void (*sig_handler)(int signum,siginfo_t *info,void *ctx);
    /* 安装某个信号*/
    int signalInstall(int signum,sig_handler handler);
    /*忽略某个信号 */
    int signalIgnore(int signum);
};

}

#endif
