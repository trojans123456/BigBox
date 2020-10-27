#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/leds.h>

struct zhem_leds
{
	int pin_num;/*引脚*/
	int led_num;/*led个数*/
	char name[8];
	struct led_classdev cdev;
};


/* 控制亮度 或开关*/
void am335x_led_brightness_set(struct led_classdev *led_cdev,
					  enum led_brightness brightness)
{
	struct zhem_leds *led = container_of(led_cdev, struct zhem_leds, cdev);
	if(!led)
	{
		printk("led is empty\n");
		return ;
	}
	printk("led = %d %d\n",led->pin_num,brightness);
	
	switch(brightness)
	{
	case LED_OFF:
		gpio_direction_output(led->pin_num,0);
		break;
	case LED_HALF:
	case LED_FULL:
		gpio_direction_output(led->pin_num,1);
		break;
	};
}

static int leds_probe(struct platform_device *pdev)
{
	printk("leds probe...\n");
    struct devcie_node *nd = pdev->dev.of_node;
    int gpio,n,i;

	struct zhem_leds *zhemleds;

    n = of_gpio_named_count(nd,"gpios");
	printk("n = %d\n",n);
	zhemleds = devm_kzalloc(&pdev->dev, sizeof(struct zhem_leds) * n,
				GFP_KERNEL);
	if(!zhemleds)
		return -ENOMEM;

	platform_set_drvdata(pdev,zhemleds);

	zhemleds->led_num = n;
	
    for(i = 0;i < n;i++)
    {
        gpio = of_get_named_gpio_flags(nd,"gpios",i,NULL);
        if(!gpio_is_valid(gpio))
        {
            printk("gpio is not valid\n");
			continue;
        }
        if(gpio_request(gpio,pdev->name) < 0)
        {
            printk("gpio request failed %d\n",gpio);
			continue;
        }
		sprintf(zhemleds[i].name,"led%d",i);
		zhemleds[i].cdev.name = zhemleds[i].name;
		zhemleds[i].cdev.default_trigger = "heartbeat";
		zhemleds[i].cdev.brightness_set = am335x_led_brightness_set;
		zhemleds[i].cdev.blink_set= am335x_led_blink_set;
		zhemleds[i].pin_num = gpio;
		
		led_classdev_register(&pdev->dev,&zhemleds[i].cdev);

    }

    return 0;
}

static int leds_remove(struct platform_device *pdev)
{
    printk("leds remvoe...\n");
	struct zhem_leds *zhemleds = platform_get_drvdata(pdev);
	int i;
	for(i = 0;i < zhemleds->led_num;i++)
		led_classdev_unregister(&(zhemleds[i].cdev));

	devm_kfree(&pdev->dev, zhemleds);
	
    return 0;
}

static struct of_device_id leds_ids[] =
{
  {.compatible = "am335x_leds"},
  {},
};

struct platform_driver am335x_leds =
{
  .probe = leds_probe,
  .remove = leds_remove,
  .driver =
    {
        .owner = THIS_MODULE,
        .name = "am335x_leds",
        .of_match_table = leds_ids,
    },
};

module_platform_driver(am335x_leds);
MODULE_LICENSE("GPL");
