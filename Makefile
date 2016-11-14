CONFIG_MODULE_SIG=n
obj-m := usb_driver.o 
 
KERNEL_DIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)

all: 
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.mod.* *.symvers *.order *~ *.cmd 
	rm -rf .usb_driver.ko.cmd .usb_driver.mod.o.cmd .usb_driver.o.cmd

