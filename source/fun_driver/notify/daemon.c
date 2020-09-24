#include <linux/module.h>
#if 0
#include <linux/notifier.h>
#else
#include "notifier.h"
#endif

/*自定义通知链*/
static RAW_NOTIFIER_HEAD(test_chain);

int register_test_notifier(struct notifier_block *nb)
{
    return raw_notifier_chain_register(&test_chain,nb);
}

EXPORT_SYMBOL(register_test_notifier);

int unregister_test_notifier(struct notifier_block *nb)
{
    return raw_notifier_chain_unregister(&test_chain,nb);
}

EXPORT_SYMBOL(unregister_test_notifier);

int test_notifier_call_chain(unsigned long val,void *v)
{
    return raw_notifier_call_chain(&test_chain,val,v);
}

static int __init init_notifier(void)
{
    return 0;
}
module_init(init_notifier);

static void __exit exit_notifier(void)
{

}
module_exit(exit_notifier);


/**** 注册 通知链 */
static int test_event1(struct notifier_block *nb,unsigned long event,void *ptr)
{
    return 0;
}

static struct notifier_block event1 =
{
    .notifier_call = &test_event1,
};

static int __init event1_init(void)
{
    register_test_notifier(&event1);
    return 0;
}
module_init(event1_init);

/*** 触发 */

/* makefile */
/**

  obj-m := xxx.o
  curpath := $(shell pwd)
  curkernel := $(shell uname -r )
  kernel_path := xxx

  all:
    make -C $(kernel_path) M=$(curpath) modules

  clean:
    make -C $(kernel_path) M=$(curpath) clean


*/
