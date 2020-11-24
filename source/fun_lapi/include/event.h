#ifndef __EVENT_H
#define __EVENT_H

#include <stdint.h>

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    EVENT_READ    =  (1UL << 0),
    EVENT_WRITE   =  (1UL << 1),
    EVENT_ERROR   =  (1UL << 2)
}event_type_t;

declear_handler(hEpoll);
declear_handler(hTmEvent);
declear_handler(hFdEvent);


/**超时*/
typedef void (*timeout_event_cb_ptr)(hTmEvent tm,void *priv);

hTmEvent timeout_event_new(int ms);
int timeout_event_delete(hTmEvent tm);

int timeout_event_setfunc(hTmEvent tm,timeout_event_cb_ptr func,void *args);
int timeout_event_add(hEpoll e,hTmEvent tm);
int timeout_event_settime(hTmEvent tm,int ms);
int timeout_event_cancel(hTmEvent tm);
int timeout_event_remaining(hTmEvent tm);


/** 普通文件 */
typedef void (*fd_event_cb_ptr)(hFdEvent fd,void *priv);

hFdEvent fd_event_new(int fd,event_type_t type);
void fd_event_delete(hFdEvent fd);

int fd_event_setfunc(hFdEvent fd,fd_event_cb_ptr func,void *args);
int fd_event_add(hEpoll e,hFdEvent fd);
int fd_event_del(hEpoll e,hFdEvent fd);
int fd_event_getevents(hFdEvent fd);


hEpoll event_loop_init(unsigned int max);
void event_loop_release(hEpoll e);
void event_loop_cancel(hEpoll e);
void event_loop_run(hEpoll e);

#ifdef __cplusplus
}
#endif

#endif
