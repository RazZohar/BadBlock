#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>


static int badblock_probe(struct usb_interface *interface, const struct usb_device_id *id){

	printk(KERN_INFO "[*] BadBlock device (%04X:%04X) is plugged\n", id->idVendor, id->idProduct);
	return 0; // return 0 will indicate that we will manage this usb_device_id
}

//badblock disconnect
static void badblock_disconnect(struct usb_interface *interface){
	printk(KERN_INFO "[*] BadBlock device is removed\n");
}

//usb_device_id
static struct usb_device_id badblock_table[] = {
	//
	{ USB_DEVICE(0x0781, 0x5406) }	, 
	{} /* terminating entry */
};
MODULE_DEVICE_TABLE(usb, badblock_table);


// usb driver
static struct usb_driver badblock_driver = {
	.name = "badblock-usb",
	.id_table = badblock_table, // usb_device_id
	.probe = badblock_probe,
	.disconnect = badblock_disconnect,
};

//initializing the badblock device driver
static int __init badblock_init(void){
	int ret = -1;

	printk(KERN_INFO "[*] BadBlock Constructor of the driver\n");
	ret = usb_register(&badblock_driver);
	printk(KERN_INFO "Registering is complete\n");

	return ret;
}

static void __exit badblock_exit(void){
	//deregister
	printk(KERN_INFO "[*] BadBlock Distructor of the driver\n");
	usb_deregister(&badblock_driver);
}

module_init(badblock_init);
module_exit(badblock_exit);
MODULE_LICENSE("GPL");
