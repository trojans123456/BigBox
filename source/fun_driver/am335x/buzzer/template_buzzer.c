#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

static int buzzer_pin_num;

int buzzer_open(struct inde *node,struct file *f)
{
	return 0;
}

int buzzer_write(struct file *f,const char __user *d,size_t l, loff_t *t)
{
	if(!d)
		return -1;
	
	switch(*d)
	{
		case 0:
			gpio_direction_output(buzzer_pin_num,0);
			break;
		case 1:
			gpio_direction_output(buzzer_pin_num,1);
			break;
		default:
			break;
	}
}

static struct file_operations buzzer_fops = 
{
	.open = buzzer_open,
	.write = buzzer_write,
};

static struct miscdevice buzzer_dev = 
{
	.minor = 240,
	.name = "am335x_buzzer",
	.fops = &buzzer_fops,
};
 
static int buzzer_probe(struct platform_device *pdev)
{
	printk("leds probe...\n");
    struct devcie_node *nd = pdev->dev.of_node;
	int n,i;

	struct zhem_leds *zhemleds;

    n = of_gpio_named_count(nd,"gpios");
	printk("n = %d\n",n);
	 

        buzzer_pin_num = of_get_named_gpio_flags(nd,"gpios",i,NULL);
        if(!gpio_is_valid(buzzer_pin_num))
        {
            printk("gpio is not valid\n");
			return -1;
        }
		if(gpio_request(buzzer_pin_num,"buzzer") < 0)
			return -1;

	misc_register(&buzzer_dev);

    return 0;
}

static int buzzer_remove(struct platform_device *pdev)
{
   gpio_free(buzzer_pin_num);
	
    return 0;
}

static struct of_device_id buzzer_ids[] = 
{
  {.compatible = "am335x_buzzer"},
  {},
};

struct platform_driver am335x_buzzer = 
{
  .probe = buzzer_probe,
  .remove = buzzer_remove,
  .driver =
    {
        .owner = THIS_MODULE,
        .name = "am335x_buzzer",
        .of_match_table = buzzer_ids,
    },
};

module_platform_driver(am335x_buzzer);
MODULE_LICENSE("GPL");