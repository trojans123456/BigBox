#ifndef __INI_CONFIG_2_H
#define __INI_CONFIG_2_H


#ifdef __cplusplus
extern "C" {
#endif

typedef struct ini__ ini_t;

ini_t *ini_load(const char *filename);
void  ini_free(ini_t *ptr);
const char *ini_get(ini_t *a_this,const char *section,const char *key);
int ini_sget(ini_t *a_this,const char *section,const char *key,const char *scanfmt, void *dst);


#ifdef __cplusplus
}
#endif

#endif
