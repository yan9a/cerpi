/////////////////////////////////////////////////////////////////////////////
// Name:         simdrv.c
// Description:  Simple device driver loadable kernel module
// Author:       Yan Naing Aye
// Date:         2020 Feb 13
// Ref: https://lwn.net/Kernel/LDD3/
/////////////////////////////////////////////////////////////////////////////

#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static int __init simdrv_init(void){
  printk(KERN_INFO "SIMDRV: init.");
  return 0;
}

static void __exit simdrv_exit(void){
  printk(KERN_INFO "SIMDRV: exit.");
}

module_init(simdrv_init);
module_exit(simdrv_exit);
