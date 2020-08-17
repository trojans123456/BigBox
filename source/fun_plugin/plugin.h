#include <stdio.h>
#ifndef __WIN32__
#include <dlfcn.h>
#endif
#include "kernel_list.h"


#ifdef __cplusplus
extern "C" {
#endif

struct version {
    int major;
    int minor;
    int patch;
};

struct plugin {
    char *name;
    char *path;
    struct version version;

    void *(*open)(void *arg);
    void (*close)(void *arg);
    void *(*call)(void *arg0, ...);

    void *handle;
    struct list_head entry;
};


typedef struct plugin_manager plugin_manager_t;

/**
 * @brief 插件管理
 * @return
 */
plugin_manager_t *plugin_manager_create();
void plugin_manager_destroy(plugin_manager_t *);


struct plugin *plugin_lookup(struct plugin_manager *pm, const char *name);
struct plugin *plugin_load(struct plugin_manager *pm, const char *path, const char *name);
void plugin_unload(struct plugin_manager *pm, const char *name);
struct plugin *plugin_reload(struct plugin_manager *pm, const char *path, const char *name);

/*
 * using HOOK_CALL(func, args...), prev/post functions can be hook into func
 */
#define HOOK_CALL(fn, ...)                                \
    ({                                                    \
        fn##_prev(__VA_ARGS__);                           \
        __typeof__(fn) *sym =  dlsym(RTLD_NEXT, #fn);     \
        if (!sym) {return NULL;}                          \
        sym(__VA_ARGS__);                                 \
        fn##_post(__VA_ARGS__);                           \
    })

/*
 * using CALL(fn, args...), you need override api
 */
#define CALL(fn, ...)                                     \
    ({__typeof__(fn) *sym = (__typeof__(fn) *)            \
                            dlsym(RTLD_NEXT, #fn);        \
     sym(__VA_ARGS__);})


#ifdef __cplusplus
}
#endif
