#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/file.h>
#include <linux/syscalls.h>



#define DEVICE_NAME "image_processor"
#define MAX_IMAGE_SIZE (1024 * 1024)
#define IMAGE_PATH "/home/abdullah/Desktop/Image-Processor/kernelspace/processed_image.raw"


static int major_number;
static char *shared_memory;

static int save_image_to_file(char *image, int height, int width) {
    struct file *filp;
    loff_t pos = 0;
    int ret;

    // Open file for writing
    filp = filp_open(IMAGE_PATH, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (IS_ERR(filp)) {
        printk(KERN_ALERT "Failed to open file for writing\n");
        return PTR_ERR(filp);
    }

    // Write image dimensions first
    ret = kernel_write(filp, &height, sizeof(int), &pos);
    if (ret < 0) {
        printk(KERN_ALERT "Failed to write height to file\n");
        filp_close(filp, NULL);
        return ret;
    }

    ret = kernel_write(filp, &width, sizeof(int), &pos);
    if (ret < 0) {
        printk(KERN_ALERT "Failed to write width to file\n");
        filp_close(filp, NULL);
        return ret;
    }

    // Write image data
    ret = kernel_write(filp, image, height * width, &pos);
    if (ret < 0) {
        printk(KERN_ALERT "Failed to write image data to file\n");
        filp_close(filp, NULL);
        return ret;
    }

    // Close file
    filp_close(filp, NULL);

    printk(KERN_INFO "Image saved successfully. Height: %d, Width: %d\n", height, width);
    return 0;
}

static int apply_image_filter(char *image, int height, int width) {
    // Simple grayscale intensity adjustment
    int i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            int index = i * width + j;
            image[index] = image[index] / 2 + 64; // Adjust brightness
        }
    }

    // Save the processed image
    save_image_to_file(image, height, width);

    return 0;
}

// ... (rest of the previous implementation remains the same)

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    int height, width;
    
    if (len > MAX_IMAGE_SIZE)
        len = MAX_IMAGE_SIZE;
    
    if (copy_from_user(&height, buffer, sizeof(int)) != 0 ||
        copy_from_user(&width, buffer + sizeof(int), sizeof(int)) != 0 ||
        copy_from_user(shared_memory + 2*sizeof(int), buffer + 2*sizeof(int), len - 2*sizeof(int)) != 0) {
        printk(KERN_ALERT "Failed to copy data from user\n");
        return -EFAULT;
    }
    
    apply_image_filter(shared_memory + 2*sizeof(int), height, width);
    
    return len;
}


static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Image Processor Device Opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Image Processor Device Closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    int bytes_read = 0;
    
    if (len > MAX_IMAGE_SIZE)
        len = MAX_IMAGE_SIZE;
    
    bytes_read = copy_to_user(buffer, shared_memory, len);
    
    return bytes_read;
}


static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init image_processor_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    
    if (major_number < 0) {
        printk(KERN_ALERT "Image Processor Module failed to register\n");
        return major_number;
    }
    
    shared_memory = kmalloc(MAX_IMAGE_SIZE, GFP_KERNEL);
    
    printk(KERN_INFO "Image Processor Module Registered\n");
    return 0;
}

static void __exit image_processor_exit(void) {
    kfree(shared_memory);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Image Processor Module Unloaded\n");
}

module_init(image_processor_init);
module_exit(image_processor_exit);
MODULE_LICENSE("GPL");