#ifndef __TASK_H
#define __TASK_H

#include <string>

using std::string;

namespace ns_core {


class Task
{
public:
    static void taskCreate( const string &path, int priority, void *(*start_routine)(void *), void *arg, int wtm = 5,pthread_attr_t *pAttr = NULL, bool *startSt = NULL );
    static void taskDestroy( const string &path, bool *endrunCntr = NULL,int wtm = 5);
    static void taskSleep(unsigned int ms);
    static void setThreadName(string name);
};

}

#endif
