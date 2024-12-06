#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "image_driver"
#define CLASS_NAME  "img_driver"

MODULE_LICENSE("GPL");

static int majorNumber;
static struct class*  imgDriverClass  = NULL;
static struct device* imgDriverDevice = NULL;

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init image_driver_init(void) {
    printk(KERN_INFO "Image Driver: Initializing\n");

    // Register major number dynamically
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "Image Driver failed to register major number\n");
        return majorNumber;
    }

    // Register device class
    imgDriverClass = class_create(CLASS_NAME);
    if (IS_ERR(imgDriverClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(imgDriverClass);
    }

    // Create device
    imgDriverDevice = device_create(imgDriverClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(imgDriverDevice)) {
        class_destroy(imgDriverClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(imgDriverDevice);
    }

    printk(KERN_INFO "Image Driver: Successfully initialized\n");
    return 0;
}

static void __exit image_driver_exit(void) {
    device_destroy(imgDriverClass, MKDEV(majorNumber, 0));
    class_unregister(imgDriverClass);
    class_destroy(imgDriverClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "Image Driver: Goodbye\n");
}

static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Image Driver: Device opened\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    // Implement read logic here
    printk(KERN_INFO "Image Driver: Read operation\n");
    return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    // Implement write logic here
    printk(KERN_INFO "Image Driver: Write operation\n");
    return len;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Image Driver: Device closed\n");
    return 0;
}

module_init(image_driver_init);
module_exit(image_driver_exit);