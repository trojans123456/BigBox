#include <stdio.h>
#include <stdlib.h>

#include "plugin.h"

struct plugin_info {
    char *path;
    char *name;
};

struct plugin_info p_info[] = {
    {"./modules/plugin_test1.so", "plugin_test1"},
    {"./modules/plugin_test2.so", "plugin_test2"},
    {"./modules/plugin_test3.so", "plugin_test3"},
};

#define SIZEOF(array)       (sizeof(array)/sizeof(array[0]))
static struct plugin_manager *pm = NULL;
void init()
{
    int i = 0;
    struct plugin *p = NULL;
    pm = plugin_manager_create();
    if (!pm) {
        printf("plugin_manager_create failed!\n");
        return;
    }
    for (i = 0; i < SIZEOF(p_info); i++) {
        p = plugin_load(pm, p_info[i].path, p_info[i].name);
        if (!p) {
            printf("plugin_load failed!\n");
            return;
        }
    }
}

void deinit()
{
    int i = 0;
    for (i = 0; i < SIZEOF(p_info); i++) {
        plugin_unload(pm, p_info[i].name);
    }
    plugin_manager_destroy(pm);
}

void foo()
{
    char *name = "plugin_test3";
    struct plugin *p = plugin_lookup(pm, name);
    if (!p) {
        printf("plugin_lookup %s failed!\n", name);
        return;
    }
    p->open(NULL);
    printf("name=%s, version=%d,%d,%d\n", p->name, p->version.major, p->version.minor, p->version.patch);
}

int main(int argc, char **argv)
{
    init();
    foo();
    deinit();
    return 0;
}


