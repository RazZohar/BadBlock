#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

#include <linux/types.h>

#include <linux/fs.h>
#include <linux/string.h>
#include <asm/uaccess.h> 

#include <linux/time.h>


//Mass storage device
#define B_INTERFACE_CLASS 8

//SCSI Sub class interface
#define B_INTERFACE_SUB_CLASS 6

//BULK only Protocol
#define B_INTERFACE_PROTOCOL 80

#define NUM_OF_BYTE_BUFF 256

MODULE_AUTHOR("Raz Zohar");
MODULE_DESCRIPTION("Module for Genral DOK");
MODULE_LICENSE("GPL");

// //char * product;
//   char * manufacturer;
//   char * serial;

char buffer[NUM_OF_BYTE_BUFF] = {0};
static struct file *log_file = NULL;

//---------------------------------------------------------------------------------------------------
static void write(struct file * filp, char * buffer)
{
	//Write to file
	filp->f_op->write(filp, buffer, strlen(buffer), &filp->f_pos);
	

}

static void log(struct file * log_files, char * file_name)
{
	mm_segment_t old_fs;

	log_files = filp_open(file_name, O_RDWR | O_CREAT, 0644);

	if(IS_ERR(log_files))
	{
		printk("open error...\n");

	}
	

	old_fs = get_fs();
	set_fs(get_ds());


	write(log_files, buffer);

	filp_close(log_files, NULL);
}




//---------------------------------------------------------------------------------------------------
//When The USB device is plug in
static int usb_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_device * usb_device_data = interface_to_usbdev(interface);

	char file_name[NUM_OF_BYTE_BUFF];

	sprintf(file_name, "/root/%s_log.txt", usb_device_data->product);

	printk("log file name:%s\n", file_name);
	printk("USB device is plugged in with product name:%s \n", usb_device_data->product);

	sprintf(buffer, "USB Device Connected:\nProduct Name:%s\t Serial Number:%s\n", usb_device_data->product, usb_device_data->serial);

	log(log_file, file_name);


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
