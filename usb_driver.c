#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

//Mass storage device
#define B_INTERFACE_CLASS 8

//SCSI Sub class interface
#define B_INTERFACE_SUB_CLASS 6

//BULK only Protocol
#define B_INTERFACE_PROTOCOL 80

const int bInterfaceClass = 8;	
const int bInterfaceSubClass = 6;	//SCSI Sub class interface


//When The USB device is plug in
static int usb_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	printk("USB device is plugged in\n");
	return 0; 
}

//When the USB device is disconnected
static void usb_disconnect(struct usb_interface *interface)
{
	printk("USB device is disconnected\n");
}

//usb device id  paramterss
static struct usb_device_id usb_table[] = 
{
	//USB_INTERFACE_INFO
	{ USB_INTERFACE_INFO(B_INTERFACE_CLASS, B_INTERFACE_SUB_CLASS, B_INTERFACE_PROTOCOL) }	, 
	{} /* terminating entry */
};

MODULE_DEVICE_TABLE(usb, usb_table);


// usb driver Deatils such as name ID table and more
static struct usb_driver usb_driver = 
{
	.name = "USB Driver",
	.id_table = usb_table, // usb_device_id
	.probe = usb_probe,
	.disconnect = usb_disconnect,
};

//initializing the usb driver
static int __init usb_init(void)
{
	int ret = -1;
	
	printk("Loading the device driver to the Kernal...\n");

	ret = usb_register(&usb_driver);

	printk("Loading process of the driver has been done sucessfully");

	return ret;
}

//Exit the USb driver
static void __exit usb_exit(void)
{
	
	printk(KERN_INFO "Remove the device driver...\n");
	usb_deregister(&usb_driver);
	printk("Removing process of the driver has been done sucessfully");
}

module_init(usb_init);
module_exit(usb_exit);
MODULE_LICENSE("GPL");
