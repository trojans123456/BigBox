#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "plugin.h"

struct plugin_manager {
    struct list_head plugins;
};

plugin_manager_t *plugin_manager_create()
{
    struct plugin_manager *pm = calloc(1, sizeof(struct plugin_manager));
    if (!pm) {
        printf("malloc failed!\n");
        return NULL;
    }
    INIT_LIST_HEAD(&pm->plugins);
    return pm;
}

void plugin_manager_destroy(plugin_manager_t *pm)
{
    if (!pm) {
        return;
    }
}

static void *plugin_get_func(struct plugin *p, const char *name)
{
    if (!p || !name) {
        return NULL;
    }
    struct plugin *q = dlsym(p->handle, name);
    if (!q) {
        printf("dlsym failed:%s\n", dlerror());
        return NULL;
    }
    return q;
}


struct plugin *plugin_lookup(struct plugin_manager *pm, const char *name)
{
    if (!pm || !name)
        return NULL;

    struct list_head* head = NULL;
    struct plugin* p = NULL;

    list_for_each(head, &pm->plugins) {
        p = list_entry(head, struct plugin, entry);
        if (0 == strcmp(p->name, name)) {
            return plugin_get_func(p, name);
        }
    }

    return NULL;
}

struct plugin *plugin_load(struct plugin_manager *pm, const char *path, const char *name)
{
    struct plugin *sym = NULL;
    struct plugin *p = NULL;
    void *handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        printf("dlopen failed: %s\n", dlerror());
        goto failed;
    }
    p = plugin_lookup(pm, name);
    if (p) {
        printf("plugin %s has already loaded!\n", name);
        return p;
    }
    sym = dlsym(handle, name);
    if (!sym) {
        printf("incompatible plugin, dlsym failed: %s\n", dlerror());
        goto failed;
    }
    p = calloc(1, sizeof(struct plugin));
    if (!p) {
        goto failed;
    }
    p->handle = handle;
    p->name = strdup(name);
    p->path = strdup(path);
    list_add(&p->entry, &pm->plugins);
    return p;

failed:
    if (handle) dlclose(handle);
    if (p) {
        free(p->name);
        free(p->path);
        free(p);
    }
    return NULL;
}

void plugin_unload(struct plugin_manager *pm, const char *name)
{
    struct plugin *p, *tmp;
    list_for_each_entry_safe(p, tmp, &pm->plugins, entry) {

        dlclose(p->handle);
        list_del(&p->entry);
        free(p->name);
        free(p->path);
        free(p);
    }
}

struct plugin *plugin_reload(struct plugin_manager *pm, const char *path, const char *name)
{
    plugin_unload(pm, name);
    return plugin_load(pm, path, name);
}