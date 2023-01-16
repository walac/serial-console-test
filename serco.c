#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/console.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define FIRST_MINOR 0
#define NUM_MINORS 1

static dev_t dev;
static struct cdev cdev;
static struct console *cons;

static ssize_t serco_write(struct file *filp, const char __user *buf,
        size_t count, loff_t *pos)
{
    char *dest;

    dest = kmalloc(count+1, GFP_KERNEL);
    if (!dest) {
        pr_err("No memory available to allocate %zu bytes\n", count);
        return -ENOMEM;
    }

    if (copy_from_user(dest, buf, count)) {
        kfree(dest);
        pr_err("copy_from_user cannot copy all bytes\n");
        return -EPERM;
    }

    if (cons) {
        cons->write(cons, dest, count);
    } else {
        dest[count] = '\0';
        pr_info("%s\n", dest);
    }

    kfree(dest);

    *pos += count;
    return count;
}

static int serco_open(struct inode *inode, struct file *filp)
{
    return nonseekable_open(inode, filp);
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = serco_write,
    .llseek = no_llseek,
    .open = serco_open,
};

static int __init serco_init(void)
{
    int err;

    /* look for the serial console driver */
    for_each_console(cons)
        if (strstarts(cons->name, "ttyS"))
            break;

    if (!cons)
        pr_warn("Could not find a serial console\n");

    err = alloc_chrdev_region(&dev, FIRST_MINOR, NUM_MINORS, "serco");
    if (err < 0) {
        pr_err("Could not allocate a device region: %d\n", err);
        return err;
    }

    cdev_init(&cdev, &fops);
    cdev.owner = THIS_MODULE;
    err = cdev_add(&cdev, dev, NUM_MINORS);
    if (err < 0) {
        unregister_chrdev_region(dev, NUM_MINORS);
        pr_err("Unable to add cdev: %d\n", err);
        return err;
    }

    return 0;
}

static void __exit serco_exit(void)
{
    cdev_del(&cdev);
    unregister_chrdev_region(dev, NUM_MINORS);
}

module_init(serco_init);
module_exit(serco_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Wander Lairson Costa");
MODULE_DESCRIPTION("Utility module to test the serial port console");
