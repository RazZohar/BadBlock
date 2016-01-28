#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
 
#define MIN(a,b) (((a) <= (b)) ? (a) : (b))
#define BULK_EP_OUT 0x01
#define BULK_EP_IN 0x82
#define MAX_PKT_SIZE 512

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maxwell Knight");
MODULE_DESCRIPTION("BadBlock USB-Attacks blocker.");
 
static struct usb_device *device;
static struct usb_class_driver class;
static unsigned char bulk_buf[MAX_PKT_SIZE];
 
static int badblock_open(struct inode *i, struct file *f){
    return 0;
}
static int badblock_close(struct inode *i, struct file *f){
    return 0;
}
static ssize_t badblock_read(struct file *f, char __user *buf, size_t cnt, loff_t *off){
    int retval;
    int read_cnt;
 
    /* Read the data from the bulk endpoint */
    retval = usb_bulk_msg(device, usb_rcvbulkpipe(device, BULK_EP_IN),
            bulk_buf, MAX_PKT_SIZE, &read_cnt, 5000);
    if (retval)
    {
        printk(KERN_ERR "Bulk message returned %d\n", retval);
        return retval;
    }
    if (copy_to_user(buf, bulk_buf, MIN(cnt, read_cnt)))
    {
        return -EFAULT;
    }
 
    return MIN(cnt, read_cnt);
}
static ssize_t badblock_write(struct file *f, const char __user *buf, size_t cnt, loff_t *off){
    int retval;
    int wrote_cnt = MIN(cnt, MAX_PKT_SIZE);
 
    if (copy_from_user(bulk_buf, buf, MIN(cnt, MAX_PKT_SIZE)))
    {
        return -EFAULT;
    }
 
    /* Write the data into the bulk endpoint */
    retval = usb_bulk_msg(device, usb_sndbulkpipe(device, BULK_EP_OUT),
            bulk_buf, MIN(cnt, MAX_PKT_SIZE), &wrote_cnt, 5000);
    if (retval)
    {
        printk(KERN_ERR "Bulk message returned %d\n", retval);
        return retval;
    }
 
    return wrote_cnt;
}
 
static struct file_operations fops = {
    .open = badblock_open,
    .release = badblock_close,
    .read = badblock_read,
    .write = badblock_write,
};
 
static int badblock_probe(struct usb_interface *interface, const struct usb_device_id *id){
    int retval;
 
    device = interface_to_usbdev(interface);
 
    class.name = "usb/block%d";
    class.fops = &fops;
    if ((retval = usb_register_dev(interface, &class)) < 0)
    {
        /* Something prevented us from registering this driver */
        printk(KERN_ERR "Not able to get a minor for this device.");
    }
    else
    {
    	printk(KERN_INFO "BadBlock-USB devcie (%04X:%04X) is plugged\n", id->idVendor, id->idProduct);
        printk(KERN_INFO "Minor obtained: %d\n", interface->minor);
    }
 
    return retval;
}
 
static void badblock_disconnect(struct usb_interface *interface){
    usb_deregister_dev(interface, &class);
    printk(KERN_INFO "BadBlock-USB device unplugged\n");
}
 
/* Table of devices that work with this driver */
static struct usb_device_id badblock_table[] = {
    { USB_DEVICE(0x0781, 0x5406) },
    {} /* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, badblock_table);
 
static struct usb_driver badblock_driver = {
    .name = "badblock_driver",
    .probe = badblock_probe,
    .disconnect = badblock_disconnect,
    .id_table = badblock_table,
};
 
static int __init badblock_init(void){
    int result;
 
    /* Register this driver with the USB subsystem */
    if ((result = usb_register(&badblock_driver)))
    {
        printk(KERN_ERR "usb_register failed. Error number %d", result);
    }else
    	printk(KERN_INFO "BadBlock-USB device registeration completed\n");

    return result;
}
 
static void __exit badblock_exit(void){
    /* Deregister this driver with the USB subsystem */
    usb_deregister(&badblock_driver);
    printk(KERN_INFO "BadBlock-USB module removed\n");
}
 
module_init(badblock_init);
module_exit(badblock_exit);