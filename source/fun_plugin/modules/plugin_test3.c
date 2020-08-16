#include "../plugin.h"

static void *test3_open(void *arg)
{
    printf("test3_open\n");
    return NULL;
}

struct plugin plugin_test3 = {
    .name = "plugin_test3",
    .version = {1, 0, 0},
    .open = test1_open,
};
