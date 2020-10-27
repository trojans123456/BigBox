#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>

#include <linux/watchdog.h>

struct zhem_wdt
{
	int pin_num; 
	int wdt_num;
	struct watchdog_device dev;
};

int am335x_wdt_ops_stop(struct watchdog_device *dev)
{
	struct zhem_wdt *wdt = watchdog_get_drvdata(dev);

	gpio_free(wdt->pin_num);
	return 0;
}

int am335x_wdt_ops_ping(struct watchdog_device *dev)
{
	struct zhem_wdt *wdt = watchdog_get_drvdata(dev);

	gpio_direction_output(wdt->pin_num,1);
	return 0;
}

int am335x_wdt_ops_start(struct watchdog_device *dev)
{
	struct zhem_wdt *wdt = watchdog_get_drvdata(dev);
	
	if(gpio_request(wdt->pin_num,"")< 0)
	{
		printk("request gpio %d failed\n",wdt->pin_num);
	}
	return 0;
}

static struct watchdog_ops am335x_wdt_ops = 
{
	.start = am335x_wdt_ops_start,
	.stop =  am335x_wdt_ops_stop,
	.ping = am335x_wdt_ops_ping,
};

static struct watchdog_info am335x_wdt_info = {
	.options =  WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING,
	.firmware_version = 1,
	.identity= "am335x_wdt_info",
};

static int wdt_probe(struct platform_device *pdev)
{
	printk("wdt probe \n");
	struct device_node *nd = pdev->dev.of_node;
	int n,i,gpio;
	struct zhem_wdt *zhemwdt;
	
	n = of_gpio_named_count(nd,"gpios");
	printk("n = %d\n",n);

	zhemwdt = devm_kzalloc(&pdev->dev, sizeof(struct zhem_wdt) * n,
				GFP_KERNEL);
	if(!zhemwdt)
		return -ENOMEM;

	platform_set_drvdata(pdev,zhemwdt);

	zhemwdt->wdt_num = n;
	
    for(i = 0;i < n;i++)
    {
        gpio = of_get_named_gpio_flags(nd,"gpios",i,NULL);
        if(!gpio_is_valid(gpio))
        {
            printk("gpio is not valid\n");
			continue;
        }
        zhemwdt[i].pin_num = gpio;
		zhemwdt[i].dev.info = &am335x_wdt_info;
		zhemwdt[i].dev.ops = &am335x_wdt_ops;
		watchdog_set_drvdata(&(zhemwdt[i].dev),&zhemwdt[i]);
		watchdog_register_device(&zhemwdt[i].dev);
    }
	
	
	return 0;
}

static int wdt_remove(struct platform_device *pdev)
{
	printk("wdt remove \n");
	struct zhem_wdt *wdt = platform_get_drvdata(pdev);
	int i;

	for(i = 0;i < wdt->wdt_num;i++)
	{
		watchdog_unregister_device(&wdt[i].dev);
	}

	devm_kfree(&pdev->dev,wdt);
	
	return 0;
}

static struct of_device_id wdt_ids[] = 
{
	{.compatible = "am335x_watchdog"},
	  {},
};

static struct platform_driver am335x_watchdog = 
{
	.probe = wdt_probe,
	.remove = wdt_remove,
	.driver = 
	{
		.owner = THIS_MODULE,
		.name = "am335x_watchdog",
		.of_match_table = wdt_ids,
	},
};

module_platform_driver(am335x_watchdog);
MODULE_LICENSE("GPL");