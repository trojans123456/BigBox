#ifndef __WATCHDOG_H_
#define __WATCHDOG_H_

#ifdef __cplusplus
extern "C" {
#endif

int wdt_start(const char *wdt_name);
void wdt_stop(int fd);
int wdt_get_timeout(int fd,int *timeout);
int wdt_set_timeout(int fd,int timeout);
int wdt_keepalive(int fd);

#ifdef __cplusplus
}
#endif

#endif // E_WATCHDOG_H_
