#ifndef __FREE_H
#define __FREE_H

#ifdef __cplusplus
extern "C" {
#endif

int sysinfo_free(void);

/**
 * @brief 获取CPU负载率
 * @return
 */
float get_cpuUsage(void);

/**
 * @brief 获取总的CPU时间
 * @return
 */
unsigned long get_cpu_total_occupy();



#ifdef __cplusplus
}
#endif

#endif
