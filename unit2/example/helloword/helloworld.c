#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init ini_module(void)
{
	printk(KERN_INFO "Hello world\n");
	return 0;
}

static void __exit cleanu_module(void)
{
	printk(KERN_INFO "Goodbye world\n");
}

module_init(ini_module);
module_exit(cleanu_module);
