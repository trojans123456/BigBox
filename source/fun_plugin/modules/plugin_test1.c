#include "../plugin.h"

static void *test1_open(void *arg)
{
    printf("test1_open\n");
    return NULL;
}

struct plugin plugin_test1 = {
    .name = "plugin_test1",
    .version = {1, 0, 0},
    .open = test1_open,
};
