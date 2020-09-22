#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/fs.h>
#include<linux/uaccess.h>  
#include <linux/slab.h> //kmaloc

#define mem_size        1024


// Variables

int CHARDRIVER_MINOR;
int CHARDRIVER_MAJOR;
char DEVICE_NAME[] = "chardriver";

dev_t dev = 0;
static struct class *dev_class;
static struct cdev chardriver_cdev;
uint8_t *kernel_buffer;

// Function definitions

static int __init chardriver_init(void);
static void __exit chardriver_exit(void);
static int chardriver_alloc_region_dinamically(void);
static int chardriver_create_struct_class(void);
static int chardriver_create_device_on_dev(void);

// File Operations
static int chardriver_open(struct inode *inode, struct file *file);
static int chardriver_release(struct inode *inode, struct file *file);
static ssize_t chardriver_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t chardriver_write(struct file *filp, const char *buf, size_t len, loff_t *off);

// File operations that points to my functions
static struct file_operations fops =
    {
        .owner = THIS_MODULE,
        .read = chardriver_read,
        .write = chardriver_write,
        .open = chardriver_open,
        .release = chardriver_release,
};

// Driver functions
static int chardriver_open(struct inode *inode, struct file *file)
{
    /*Creating Physical memory*/
    if((kernel_buffer = kmalloc(mem_size , GFP_KERNEL)) == 0){
        printk(KERN_INFO "Cannot allocate memory in kernel\n");
        return -1;
    }
    printk(KERN_INFO "Device File Opened...!!!\n");

    return 0;
    
}

static int chardriver_release(struct inode *inode, struct file *file)
{
    kfree(kernel_buffer);
    printk(KERN_INFO "Device File Closed...!!!\n");
    return 0;
}

static ssize_t chardriver_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    copy_to_user(buf, kernel_buffer, mem_size);
    printk(KERN_INFO "Data Read : Done!\n");
    return mem_size;
}
static ssize_t chardriver_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    copy_from_user(kernel_buffer, buf, len);
    printk(KERN_INFO "Data Write : Done!\n");
    return len;
}

// Setup functions

static int
chardriver_create_struct_class(void)
{
    /*Creating struct class*/
    if ((dev_class = class_create(THIS_MODULE, "chardriver_class")) == NULL)
    {
        printk(KERN_INFO "Cannot create the struct class for device\n");
        return -1;
    }
    return 0;
}

static int chardriver_create_device_on_dev(void)
{
    if ((device_create(dev_class, NULL, dev, NULL, DEVICE_NAME)) == NULL)
    {
        printk(KERN_INFO "Cannot create the Device\n");
        return -1;
    }

    return 0;
}

static int chardriver_alloc_region_dinamically(void)
{
    int result = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (result < 0)
    {
        printk(KERN_INFO "Cannot allocate major number for device 1\n");
        return -1;
    }
    CHARDRIVER_MAJOR = MAJOR(dev);
    CHARDRIVER_MINOR = MINOR(dev);

    printk(KERN_INFO "Major = %d Minor = %d \n", CHARDRIVER_MAJOR, CHARDRIVER_MINOR);
    return 0;
}

static int chardriver_add_cdev(void)
{
    /*Creating cdev structure*/
    cdev_init(&chardriver_cdev, &fops);

    /*Adding character device to the system*/
    if ((cdev_add(&chardriver_cdev, dev, 1)) < 0)
    {
        printk(KERN_INFO "Fail when trying to add cdev.");

        return -1;
    }
    printk(KERN_INFO "Cdev Added");

    return 0;
}

// Init and Exit functions

static __init int chardriver_init(void)
{
    if (chardriver_alloc_region_dinamically() < 0)
    {
        return -1;
    }
    if (chardriver_add_cdev() < 0)
    {
        goto r_device;
    }
    if (chardriver_create_struct_class() < 0)
    {
        goto r_class;
    }
    if (chardriver_create_device_on_dev() < 0)
    {
        goto r_device;
    }

    printk(KERN_INFO "Kernel Module Inserted Successfully...\n");

    return 0;

r_class:
    printk(KERN_INFO "Unregistering class because an error..\n");
    unregister_chrdev_region(dev, 1);
    return -1;

r_device:
    printk(KERN_INFO "Removing device because an error..\n");
    class_destroy(dev_class);
    return -1;
}

static void __exit chardriver_exit(void)
{
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&chardriver_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Kernel Module Removed Successfully...\n");
}

module_init(chardriver_init);
module_exit(chardriver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Caio Farias");
MODULE_DESCRIPTION("Simple Character Driver");
MODULE_VERSION("1.5");