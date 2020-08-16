#include "../plugin.h"

static void *test2_open(void *arg)
{
    printf("test2_open\n");
    return NULL;
}

struct plugin plugin_test2 = {
    .name = "plugin_test2",
    .version = {1, 0, 0},
    .open = test1_open,
};
