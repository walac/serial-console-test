#define pr_fmt(fmt) "%s: " fmt, KBUILD_MODNAME
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/console.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>

static struct console *cons;

static ssize_t serco_write(struct file *filp, const char __user *buf,
			   size_t count, loff_t *pos);

static int serco_open(struct inode *inode, struct file *filp)
{
	return nonseekable_open(inode, filp);
}

static const struct file_operations serco_fops = {
	.owner = THIS_MODULE,
	.write = serco_write,
	.llseek = no_llseek,
	.open = serco_open,
};

static struct miscdevice serco_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = KBUILD_MODNAME,
	.mode = 0666,
	.fops = &serco_fops,
};

static ssize_t serco_write(struct file *filp, const char __user *buf,
			   size_t count, loff_t *pos)
{
	char *dest;
	struct device *dev = serco_miscdev.this_device;

	dest = kmalloc(count+1, GFP_KERNEL);
	if (!dest)
		return -ENOMEM;

	if (copy_from_user(dest, buf, count)) {
		kfree(dest);
		dev_dbg(dev, "copy_from_user cannot copy all bytes\n");
		return -EPERM;
	}

	if (cons) {
		cons->write(cons, dest, count);
	} else {
		dest[count] = '\0';
		dev_info(dev, "%s\n", dest);
	}

	kfree(dest);

	*pos += count;
	return count;
}

static int __init serco_init(void)
{
	int ret;

	ret = misc_register(&serco_miscdev);
	if (ret)
		return ret;

	/* look for the serial console driver */
	for_each_console(cons)
		if (strstarts(cons->name, "ttyS"))
			break;

	if (!cons)
		dev_warn(serco_miscdev.this_device, "Could not find a serial console\n");

	return 0;

}

static void __exit serco_exit(void)
{
	misc_deregister(&serco_miscdev);
}

module_init(serco_init);
module_exit(serco_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Wander Lairson Costa");
MODULE_DESCRIPTION("Utility module to test the serial port console");
