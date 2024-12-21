#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/printk.h>

MODULE_AUTHOR("Mihao");
MODULE_DESCRIPTION("Tomsk State University Kernel Module");

static int __init tsu_init(void) {
  printk(KERN_INFO "Welcome to the Tomsk State University\n");
  return 0;
}

static void __exit tsu_exit(void) {
  printk(KERN_INFO "Tomsk State University say goodbye!\n");
}

module_init(tsu_init);
module_exit(tsu_exit);
MODULE_LICENSE("GPL");