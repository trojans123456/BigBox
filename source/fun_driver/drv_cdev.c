/*linux 内核使用cdev描述一个字符设备

    struct cdev
    {
        struct kobject kobj;
        struct module *owner;
        struct file_operations *ops;
        struct list_head list;
        dev_t dev; //设备号
        unsigned int count;
    };

    //获取主次设备号
    MAJOR(dev_t dev)
    MINOR(dev_t dev)

    //生成主设备号
    MKDEV(int major,int minor)

*/


struct test_cdev
{
    struct cdev cdev;
    int dev_no;
};

struct test_cdev g_cdev;

ssize_t test_read(struct file *filp,char __user *buf,size_t count,loff_t *f_ops)
{
    copy_to_user(buf,...,...);
}

ssize_t test_write(struct file *filp,const char __user *buf,size_t count,loff_t *f_ops)
{
    copy_from_user(...,buf,...);
}

long test_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
    switch(cmd)
    {
    case CMD1:
        break;
    case CMD2:
        break;
    default:
        return -ENOTTY;
    }

    return 0;
}

static struct file_operations g_ops =
{
    .owner = THIS_MODULE,
    .read = test_read,
    .write = test_write,
    .unlocked_ioctl = test_ioctl
            /*open release */
};

static int __init test_cdev_init(void)
{
    cdev_init(&g_cdev.cdev,&g_ops);
    g_cdev.cdev.owner = THIS_MODULE;

    /*获取设备号*/
    if(0)
    {
        register_chrdev_region(g_cdev.dev_no,1,DEV_NAME);
    }
    else
    {
        alloc_chrdev_region(&g_cdev.dev_no,0,1,DEV_NAME);
    }

    cdev_add(&g_cdev.cdev,xxx_dev_no,1); /*注册设备 */
}

module_init(test_cdev_init);

static void __exit test_cdev_exit(void)
{
    unregister_chrdev_region(g_cdev.dev_no,1);/*释放占用的设备号*/
    cdev_del(&g_cdev.cdev);
}
