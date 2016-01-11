obj-m := usb_driver.o badblock_usb_module.o

KERNEL_DIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)

all: 
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules

install_raz:usb_driver.ko
	sudo rmmod usb_storage | sudo insmod usb_driver.ko	

remove_raz:
	sudo rmmod usb_driver

install_maxwell:badblock_usb_module.ko
	sudo rmmod usb_storage | sudo insmod badblock_usb_module.ko	

remove_maxwell:
	sudo rmmod badblock_usb_module

clean:
	rm -rf *.o *.mod.* *.symvers *.order *~ *.cmd ./.*
