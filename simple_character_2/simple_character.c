#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "simple_char"
#define CLASS_NAME "simple_char_device"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TIENLUONG");
MODULE_DESCRIPTION("A simple Linux char driver");
MODULE_VERSION("0.1");

static int      major_number;
static char     message[256];
static char*    tempmess;
static short    size_of_message;
static int      numberOpens = 0;
static struct   class*  char_class;
static struct   device* char_device;

static int      dev_open (struct inode *, struct file *);
static int      dev_release (struct inode *,struct file *);
static ssize_t  dev_read (struct file *, char *, size_t, loff_t *);
static ssize_t  dev_write (struct file *,const char *, size_t, loff_t *);

static struct file_operations fops =
{
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init char_init(void)
{
    tempmess = message;
    printk(KERN_INFO "%s: Khoi tao driver\n",__func__);

    //Config GPIO o day

    /* Cap phat dong major number cho char device */
    major_number = register_chrdev(0,DEVICE_NAME, &fops);
    if (0==major_number)
    {
        printk(KERN_ALERT "%s: Khong cap phat duoc major number\n",__func__);
        return major_number;
    }

    /* Dang ky class cho device */
    char_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(char_class))
    {
        printk(KERN_ALERT "%s: Khong dang ky duoc device class\n",__func__);
        unregister_chrdev(major_number,DEVICE_NAME);
        return PTR_ERR(char_class);
    }
    /* Dang ky char device */
    char_device = device_create(char_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(char_device))
    {
        printk(KERN_ALERT "%s: Khong tao duoc device\n",__func__);
        class_destroy(char_class);
        unregister_chrdev(major_number,DEVICE_NAME);
        return PTR_ERR(char_device);
    }

    printk(KERN_INFO "%s: Khoi tao thanh cong\n",__func__);
    return 0;
}

static void __exit char_exit(void)
{
    device_destroy(char_class, MKDEV(major_number,0));
    class_unregister(char_class);
    class_destroy(char_class);
    unregister_chrdev(major_number,DEVICE_NAME);
    printk(KERN_INFO "%s: Driver exit\n",__func__);
}

static int dev_open (struct inode *inodep, struct file *filep)
{
    return 0;
}

// unsigned int LED_OUT = 0x5;
// unsigned int LED_IN = 0x10;

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    int error = 0;
    int maxsize;

    maxsize = strlen(tempmess);
    if (maxsize <=0)
    {
        tempmess = message;
        return 0;
    }
    maxsize = maxsize > len ? len : maxsize;
    printk(KERN_INFO "%s: Message = %s",__func__,message);
    printk(KERN_INFO "%s: Maxsize = %d, Length = %u\n",maxsize,(unsigned int)len);
    // void __iomem *led_in_reg = ioremap(LED_IN, 4);
    //Doc tu thanh ghi nay de biet led sang hay tat
    // val = ioread32(led_in_reg);

    /* Muon ghi vao bo nho tren user phai su dung copy to user */
    error = copy_to_user(buffer, message, sizeof(message));

    if (0 != error)
    {
        printk(KERN_INFO "%s: Khong the ghi du lieu vao bo nho cua user\n",__func__);
        return -EFAULT;
    }
    printk(KERN_INFO "%s: Gui chuoi %s len user-space\n",__func__, message);
    return 0;
}

static ssize_t dev_write (struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    // void __iomem *led_out_reg = ioremap(LED_OUT, 4);
    
    // iowrite32(led_out_reg,val);
    memset(message,0,sizeof(message));
    copy_to_user(message, buffer, len);
    // sprintf(message, "%s", buffer);
    printk(KERN_INFO "%s: Da ghi chuoi %s vao message\n",__func__,buffer);

    return strlen(message);
}

static int dev_release (struct inode *inodep,struct file *filep)
{
    return 0;
}

module_init(char_init);
module_exit(char_exit);