
obj-m := template_wdt.o

KERNELDIR ?= ../
PWD = $(shell pwd)

default:
		$(MAKE) ARCH=arm CROSS_COMPILE=XXX -C $(KERNELDIR) M=$(PWD) modules
		
clean:
		-rm modules.*
		-rm *.o
		-rm *.ko
		-rm *.mod.c
		-rm Module.*