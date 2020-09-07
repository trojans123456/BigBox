

/**
 * #define MACHINE_START(_type,_name)			\
static const struct machine_desc __mach_desc_##_type	\
 __used							\
 __attribute__((__section__(".arch.info.init"))) = {	\
    .nr		= MACH_TYPE_##_type,		\
    .name		= _name,

#define MACHINE_END				\
};
 *
 */

/*实现platform device */
struct platform_device xxx_plat_dev =
{
    .resource = ....
};

static struct platform_device *xx_devices[] =
{
    /*.....*/
    &xxx_plat_dev,
};

static void __init xx_init(void)
{
    ....
    /*添加到platform*/
    platform_add_devices(xx_devices,num);
}

MACHINE_START(test,"test")
    .init_irq = xx_ir,
    .map_io = xx_io,
    .init_machine = xx_init,
    .timer = &xx_timer,
MACHINE_END



/* serial driver **********/
static struct console xx_console =
{
    ....
};

int drv_porbe(struct platform_device *dev)
{
    uart_add_one_port();
}

struct platform_driver xx_driver =
{
    .probe = drv_probe
};

static int __init xx_console_init(void)
{
    /*目前console只做串口打印输出*/
    register_console(&xx_console);

    platform_driver_register(&xx_dirver);
}

console_initcall(xx_init);


/**************************************************
---------------------------------------------------------------
    /dev/console  /dev/tty   /dev/tty0              user sapce
----------------------------------------------------------------
            vfs  (char device driver )(read write open ....)             printk
----------------------------------------------------------------          ↑
       ttyLine              ttyDevice            ttydirver      <---->  console
       Discipline            (virtual)
-----------------------------------------------------------------------
         ↓                                  ↓
        n_tty ...            VT(虚拟终端)   Serial     ....
----------------------------------------------------------------
                                            ↓
                                           uart
*/


/**
 * 终端类型
 * 1.控制台 console
 *   键盘输入 显示器输出 抽象概念 日志打印
 *
 * 2.虚拟终端 VT
 *      Ctrl + Alt F1~F6
 *
 * 3.串口终端 TTY
 *
 * 4.软件终端
 *   PuTTY SecureCRT 超级终端 ssh ...
 *
 * 命令行 console="/dev/ttyS0",console="/dev/ttyUSB0"
 *
 * /dev/console 控制台
 *
 * /dev/ttyN 虚拟终端
 *
 * /dev/pty  pts(slave) ptm(master)伪终端
 *
 * /dev/tty 控制终端
 *
 * tty device仅仅是个数据通道。并不存在。起实际由具体硬件驱动决定
 *
 * tty 驱动  tty_driver.h
 * struct tty_driver
 * {
 * };
 *
 * tty的动态抽象，从设备打开到设备关闭。
 * 代指tty设备,在tty core中保存了tty设备的整个生命周期的中间变量
 * struct tty_struct
 * {
 * };
 *
 * tty设备的静态属性。
 * struct tty_port
 * {};
 *
 * struct kermios 内核层 for 用户编程
 * {};
 *
 * tty driver 步骤
 * 1.实现 struct tty_operations ops
 * 2.调用tty_alloc_driver分配一个tty_driver
 * 3.调用tty_register_driver
 * 4.如果需要动态注册tty设备,在合适时机调用tty_register_device
 * 或tty_register_device_attr 向内核注册tty设备
 * 5.发送数据时,tty core会调用driver提供的回调
 *   接收数据时,调用tty_insert_flip_string or tty_insert_flip_char给tty core
 *
 *
 *
 * console
 * struct console 抽象的console
 * register_console 注册driver
*/
