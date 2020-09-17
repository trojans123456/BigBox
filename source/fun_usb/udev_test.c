#include <stdio.h>
#include <unistd.h>
#include <libudev.h>

int main()
{
    struct udev *udev;
    struct udev_device *dev;
    struct udev_monitor *mon;
    int fd;

    /* create udev object */
    udev = udev_new();
    if (!udev) {
        fprintf(stderr, "Can't create udev\n");
        return 1;
    }

    mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "net", NULL);
    udev_monitor_enable_receiving(mon);
    fd = udev_monitor_get_fd(mon);

    while (1) {
        fd_set fds;
        struct timeval tv;
        int ret;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        ret = select(fd+1, &fds, NULL, NULL, &tv);
        if (ret > 0 && FD_ISSET(fd, &fds)) {
            dev = udev_monitor_receive_device(mon);
            if (dev) {
                printf("I: ACTION=%s\n", udev_device_get_action(dev));
                printf("I: DEVNAME=%s\n", udev_device_get_sysname(dev));
                printf("I: DEVPATH=%s\n", udev_device_get_devpath(dev));
                printf("I: MACADDR=%s\n", udev_device_get_sysattr_value(dev, "address"));
                printf("---\n");

                /* free dev */
                udev_device_unref(dev);
            }
        }
        /* 500 milliseconds */
        usleep(500*1000);
    }
    /* free udev */
    udev_unref(udev);

    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libudev.h>

#define BLOCK_SIZE 512

int main()
{
    struct udev *udev;
    struct udev_device *dev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;

    /* create udev object */
    udev = udev_new();
    if (!udev) {
        fprintf(stderr, "Cannot create udev context.\n");
        return 1;
    }

    /* create enumerate object */
    enumerate = udev_enumerate_new(udev);
    if (!enumerate) {
        fprintf(stderr, "Cannot create enumerate context.\n");
        return 1;
    }

    udev_enumerate_add_match_subsystem(enumerate, "block");
    udev_enumerate_scan_devices(enumerate);

    /* fillup device list */
    devices = udev_enumerate_get_list_entry(enumerate);
    if (!devices) {
        fprintf(stderr, "Failed to get device list.\n");
        return 1;
    }

    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path, *tmp;
        unsigned long long disk_size = 0;
        unsigned short int block_size = BLOCK_SIZE;

        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);

        /* skip if device/disk is a partition or loop device */
        if (strncmp(udev_device_get_devtype(dev), "partition", 9) != 0 &&
            strncmp(udev_device_get_sysname(dev), "loop", 4) != 0) {
            printf("I: DEVNODE=%s\n", udev_device_get_devnode(dev));
            printf("I: KERNEL=%s\n", udev_device_get_sysname(dev));
            printf("I: DEVPATH=%s\n", udev_device_get_devpath(dev));
            printf("I: DEVTYPE=%s\n", udev_device_get_devtype(dev));

            tmp = udev_device_get_sysattr_value(dev, "size");
            if (tmp)
                disk_size = strtoull(tmp, NULL, 10);

            tmp = udev_device_get_sysattr_value(dev, "queue/logical_block_size");
            if (tmp)
                block_size = atoi(tmp);

            printf("I: DEVSIZE=");
            if (strncmp(udev_device_get_sysname(dev), "sr", 2) != 0)
                printf("%lld GB\n", (disk_size * block_size) / 1000000000);
            else
                printf("n/a\n");
        }

        /* free dev */
        udev_device_unref(dev);
    }
    /* free enumerate */
    udev_enumerate_unref(enumerate);
    /* free udev */
    udev_unref(udev);

    return 0;
}


#include <stdio.h>
#include <libudev.h>

#define SYSPATH "/sys/class/net"

int main(int argc, char *argv[])
{
    struct udev *udev;
    struct udev_device *dev, *dev_parent;
    char device[128];

    /* verify that we have an argument, like eth0, otherwise fail */
    if (!argv[1]) {
        fprintf(stderr, "Missing network interface name.\nexample: %s eth0\n", argv[0]);
        return 1;
    }

    /* build device path out of SYSPATH macro and argv[1] */
    snprintf(device, sizeof(device), "%s/%s", SYSPATH, argv[1]);

    /* create udev object */
    udev = udev_new();
    if (!udev) {
        fprintf(stderr, "Cannot create udev context.\n");
        return 1;
    }

    /* get device based on path */
    dev = udev_device_new_from_syspath(udev, device);
    if (!dev) {
        fprintf(stderr, "Failed to get device.\n");
        return 1;
    }

    printf("I: DEVNAME=%s\n", udev_device_get_sysname(dev));
    printf("I: DEVPATH=%s\n", udev_device_get_devpath(dev));
    printf("I: MACADDR=%s\n", udev_device_get_sysattr_value(dev, "address"));

    dev_parent = udev_device_get_parent(dev);
    if (dev_parent)
        printf("I: DRIVER=%s\n", udev_device_get_driver(dev_parent));

    /* free dev */
    udev_device_unref(dev);

    /* free udev */
    udev_unref(udev);

    return 0;
}
