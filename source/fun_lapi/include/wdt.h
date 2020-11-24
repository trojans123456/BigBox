#ifndef __WDT_H
#define __WDT_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

declear_handler(hWDT);

hWDT wdt_start(const char *wdt_name);
void wdt_stop(hWDT w);
int wdt_get_timeout(hWDT w,int *timeout);
int wdt_set_timeout(hWDT w,int timeout);
int wdt_keepalive(hWDT w);

#ifdef __cplusplus
}
#endif

#endif
