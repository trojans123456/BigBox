#include <stdio.h>
#include "signal.h"

using namespace ns_core;

int signal::signalInstall(int sigNum, sig_handler handler)
{
    struct sigaction sigAction;
    sigAction.sa_sigaction = handler;
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = SA_SIGINFO;
    if (sigaction(sigNum, &sigAction, NULL) != 0)
    {
        return -1;
    }
    return 0;
}

int signal::signalIgnore(int sigNum)
{
    struct sigaction sigAction;
    sigAction.sa_handler = SIG_IGN;
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = 0;
    if (sigaction(sigNum, &sigAction, NULL) != 0)
    {
        return -1;
    }
    return 0;
}
