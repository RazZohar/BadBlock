#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/semaphore.h>

#define DEVICE_NAME "badblockdevice" //name--> appears in /proc/devices
#define DATALEN 100

/* structure of the badblock device*/
struct badblock_device{
	char data[DATALEN];
	struct semaphore sem;
}badblock_device;

/* variables to later be able to register badbock device  */
struct cdev *mcdev;
int major_number; /* will store out major number - extracted from dev_t using macro */
int ret;		  /* will be used to store return values of functions */
				  /* the kernel stack is very small thats why we declaring variables all over */
				  
dev_t dev_num;	  /* will hold the major number the kernel gives us */

// called when device is opened
int badblock_open(struct inode *inode, struct file *filep){
	//allows only one process to open this device by using lock - mutex
	if(down_interruptible(&badblock_device.sem) != 0){
		printk(KERN_ALERT "badblock: could not lock device during open");
		return -1;
	}
	printk(KERN_INFO "badblock: opened device");
	return 0;
}

// called when user wants to receive information from the device
ssize_t badblock_read(struct file* filp, char* buff, size_t buff_count, loff_t* currOffset){
	//takes data from kernel(device) space and transfare it to the user space(process)
	printk(KERN_INFO "badblock: reading from device");
	ret = copy_to_user(buff, badblock_device.data, buff_count);
	return ret;
}

// called when user wants to write information info the device
ssize_t badblock_write(struct file* filp, char* buff, size_t buff_count, loff_t* currOffset){
	//sends data from user space(process) to kernel space(device)
	printk(KERN_INFO "badblock: reading from device");
	ret = copy_from_user(badblock_device.data, buff, buff_count);
	return ret;
}

// called when user closes
int badblock_close(struct inode *inode, struct file* filp){

	//when called , which is the opposite of down for semaphore, we release the mutex we obtained at device open
	up(&badblock_device.sem);
	printk(KERN_INFO "badbock: closed device");
	return 0;
}

/* tell the kernel which functions to call when user operates on our device */
struct file_operations f_ops = {
	.owner = THIS_MODULE,
	.open = badblock_open,
	.release = badblock_close,
	.write = badblock_write,
	.read = badblock_read
};

static int badbock_entry(void){
	/* register our device with the system
		1 - use dynamic allocation to assign our device a major number
		*/
	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if(ret < 0){ // if kernel function return a negative there is an error
		printk(KERN_ALERT "[*] badblock: failed to allocate a major number");
		return ret; //propagate error
	}
	major_number = MAJOR(dev_num); //extract the major number and stores it in our variable
	printk(KERN_INFO "badblock: major number is %d", major_number);
	printk(KERN_INFO "\tuse \"mknod /dev/%s c %d 0\" for device file", DEVICE_NAME, major_number);

	mcdev = cdev_alloc(); // creates out cdev structure, initializes our cdev
	mcdev->ops = &f_ops;
	mcdev->owner = THIS_MODULE;
	//after creation we must add cdev to the kernel
	ret = cdev_add(mcdev, dev_num, 1);
	if(ret < 0){
		printk(KERN_ALERT "[*] badblock: unable to load cdev to kernel");
		return ret;
	}

	//initalizes semaphore
	sema_init(&badblock_device.sem, 1); // initial value of one

	return 0;
}

static void badblock_exit(void){
	//unregister everything in a reverse order
	cdev_del(mcdev);

	unregister_chrdev_region(dev_num, 1);
	printk(KERN_ALERT "badblock: module unloaded");
}

//inform the kernel where to start and stop our module/driver
module_init(badbock_entry);
module_exit(badblock_exit);