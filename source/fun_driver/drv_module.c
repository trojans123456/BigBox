#include <linux/init.h>
#include <linux/module.h>


int test_add(int a,int b)
{
    return a + b;
}
/* export symbol for other moudle */
EXPORT_SYMBOL_GPL(test_add);

/**
  insmod ./test.ko

  lsmod => cat /proc/modules
*/

/* insmod test.ko test_name='xxx' */

static char *test_name = "";
module_param(test_name,charp,S_IRUGO);

static int __init test_init(void)
{
    return 0;
}

module_init(test_init);

static void __exit test_exit(void)
{

}

module_exit(test_exit);

MODULE_AUTHOR("XX");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A TEST ");
MODULE_ALIAS("A TEST MODULE");


/* compiler */
/**
*  Makefile
*
*   KVERS = $(shell uname -r )
*
*   #kernel modules
*   obj-m += test.o
*
*   # flags
*   EXTRA_CFLAGS = -g -O0
*
*   build: kernel_modules
*
*   kernel_modules:
*       make -C /lib/moudles/$(KVERS)/build M=$(CURDIR) modules
*
*   clean:
*       make -C /lib/moudles/$(KVERS)/build M=$(CURDIR) clean
*
*
*
* /
