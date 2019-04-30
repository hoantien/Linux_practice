#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/io.h>

MODULE_AUTHOR("Luong Hoan Tien");    /* Who wrote this module? */
MODULE_DESCRIPTION("Hello world kernel module");        /* What does this module do */
MODULE_LICENSE("GPL");

#define DEVICE_NAME	"led_dev"
#define BASE_ADDR	0x4804C000
#define LED		24
#define GPIO_OE		0x134
#define GPIO_DATAOUT	0x13C

static void __iomem *io;
static dev_t dev;
static struct cdev *my_cdev;
static struct class *class_name;
char* cmd ;
unsigned long temp;

static void set_on (int gpio)
{
	temp = ioread32(io + GPIO_DATAOUT);
	temp |= (1<<gpio);
	iowrite32(temp,io+GPIO_DATAOUT);
}


static void set_off (int gpio)
{
	temp = ioread32(io + GPIO_DATAOUT);
	temp &= ~(1<<gpio);
	iowrite32(temp,io+GPIO_DATAOUT);
}

static void read_gpio(char *data, int gpio)
{
	temp = ioread32(io+GPIO_DATAOUT);
	*data = '0'+(char)(((1<<gpio)&temp) >> gpio);
}

static int dev_open(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "TienLuong %s, %d\n", __func__, __LINE__);
	return 0;
}

static int dev_close(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "TienLuong %s, %d\n", __func__, __LINE__);
        return 0;
}

static ssize_t dev_read(struct file*filep, char __user *buf, size_t len, loff_t *offset)
{
	char data;
	ssize_t ret;
	read_gpio(&data,21);
	printk(KERN_INFO "TienLuong %s, %d\n", __func__,len);
	ret = copy_to_user(buf, &data,1);
	return (1-ret);
}

static ssize_t dev_write(struct file*filep, const char __user *buf, size_t len, loff_t *offset)
{
	printk(KERN_INFO "TienLuong %s, %d, %d\n",__func__,__LINE__, len);
	cmd = kmalloc(len,GFP_USER);
	copy_from_user(cmd,buf,len);
	if (*cmd == '1')
	{
		printk(KERN_INFO "TienLiong on\n");
		set_on(LED);
	}
	else if (*cmd == '0')
	{
		printk(KERN_INFO "TienLiong off\n");
		set_off(LED);
	}
	else {;}
	kfree(cmd);
	//msleep(100000);
	return 1;
}

static struct file_operations fops = {
	.open = dev_open,
	.release = dev_close,
	.read = dev_read,
	.write = dev_write,
};

static int __init exam_init(void)
{
	printk(KERN_INFO "TienLuong %s, %d\n", __func__, __LINE__);
	alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
	my_cdev = cdev_alloc();
	//cdev_init(&my_cdev, &fops);
	my_cdev->ops = &fops;
	my_cdev->owner = THIS_MODULE;
	cdev_add(my_cdev, dev, 1);
	class_name = class_create(THIS_MODULE, "my_dev_class");
	device_create(class_name, NULL, dev, NULL, DEVICE_NAME);

	io = ioremap(BASE_ADDR,0x1000);
	if(io == NULL)
	{
		printk(KERN_ERR "Fail map base address\n");
		return -1;
	}
	temp = ioread32(io+GPIO_OE);
	temp &= ~(1<<LED);
	iowrite32(temp,io+GPIO_OE);
	return 0;
}

static void __exit exam_exit(void)
{
	set_off(LED);
	cdev_del(my_cdev);
	device_destroy(class_name, dev);
	class_destroy(class_name);
	unregister_chrdev_region(dev, 1);
}

module_init(exam_init);
module_exit(exam_exit);

