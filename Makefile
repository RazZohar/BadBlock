+CONFIG_MODULE_SIG=n
+
 obj-m := usb_driver.o badblock_usb_module.o
 
 KERNEL_DIR = /lib/modules/$(shell uname -r)/build
@@ -19,5 +21,7 @@ remove_maxwell:
 	sudo rmmod badblock_usb_module
 
 clean:
-	rm -rf *.o *.mod.* *.symvers *.order *~ *.cmd ./.*
+	rm -rf *.o *.mod.* *.symvers *.order *~ *.cmd 
+	rm -rf .badblock_usb_module.ko.cmd .badblock_usb_module.mod.o.cmd .badblock_usb_module.o.cmd
+	rm -rf .usb_driver.ko.cmd .usb_driver.mod.o.cmd .usb_driver.o.cmd
