#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#define DEVICE_NAME "cedevdrv"
#define CLASS_NAME "cedev"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cool Emerald");
MODULE_DESCRIPTION("A Linux device driver");
MODULE_VERSION("1.0");

static char *cedrvarg = "cedriver";
module_param(cedrvarg, charp, S_IRUGO);
MODULE_PARM_DESC(cedrvarg,"To use with printk");

static int majorNumber;
static struct class* cedevdrvClass = NULL;
static struct device* cedevdrvDevice = NULL;

static int device_open(struct inode *inodep, struct file *filep);
static int device_release(struct inode *inodep, struct file *filep);
static ssize_t device_read(struct file *filep,  char *buffer,  size_t length, loff_t * offset);
static ssize_t device_write(struct file *filep, const char *buffer, size_t length, loff_t * offset);

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = device_open,
    .release = device_release,
    .read    = device_read,
    .write   = device_write
};

static char mes[256]={0};
static size_t meslen = 0;

static DEFINE_MUTEX(cedevmutex);

static int __init cedevdrv_init(void)
{
  printk(KERN_INFO "CEDRV: init %s\n",cedrvarg);
  
  // register char device
  majorNumber = register_chrdev(0,DEVICE_NAME, &fops);
  if(majorNumber<0) {
    printk(KERN_ALERT "CEDRV: registering chrdev failed\n");
    return majorNumber;
  }
  
  // create device class
  cedevdrvClass = class_create(THIS_MODULE,CLASS_NAME);
  if(IS_ERR(cedevdrvClass)){
    unregister_chrdev(majorNumber,DEVICE_NAME);
    printk(KERN_ALERT "CEDRV: creating device class failed\n");
    return PTR_ERR(cedevdrvClass);
  }
  
  // create device
  cedevdrvDevice = device_create(cedevdrvClass,NULL,MKDEV(majorNumber,0),NULL,DEVICE_NAME);
  if(IS_ERR(cedevdrvDevice)){
    class_destroy(cedevdrvClass);
    unregister_chrdev(majorNumber,DEVICE_NAME);
    printk(KERN_ALERT "CEDRV: creating device failed\n");
    return PTR_ERR(cedevdrvDevice);
  }
  printk(KERN_INFO "CEDRV: device %s created\n",cedrvarg);
  mutex_init(&cedevmutex);
  return 0;
}

static void __exit cedevdrv_exit(void)
{
  mutex_destroy(&cedevmutex);
  device_destroy(cedevdrvClass,MKDEV(majorNumber,0));
  class_unregister(cedevdrvClass);
  class_destroy(cedevdrvClass);
  unregister_chrdev(majorNumber,DEVICE_NAME);
  printk(KERN_INFO "CEDRV: exit %s\n",cedrvarg);
}

static int device_open(struct inode *inodep, struct file *filep)
{
    if(!mutex_trylock(&cedevmutex)){
        printk(KERN_ALERT "CEDRV: device is busy\n");
        return -EBUSY;
    }
    printk(KERN_INFO "CEDRV: device opened\n");
    return 0;
}

static int device_release(struct inode *inodep, struct file *filep)
{
    mutex_unlock(&cedevmutex);
    printk(KERN_INFO "CEDRV: device closed\n");
    return 0;    
}

static ssize_t device_write(struct file *filep, const char *buffer, size_t length, loff_t * offset)
{
    int i=0;
    for(i=0;i<length;i++) mes[i]=buffer[i];
    meslen = length;
    printk(KERN_INFO "CEDRV: received %zu bytes\n",length);
    return length;
}

static ssize_t device_read(struct file *filep,  char *buffer,  
    size_t length, loff_t * offset)
{
    int err_count=0;
    int sent_count=0;
    err_count = copy_to_user(buffer,mes,meslen);
    if(err_count==0){
        printk(KERN_INFO "CEDRV: sent %zu bytes\n",meslen);
        sent_count=meslen;
        meslen=0;//reset len
        return sent_count;
    }
    else {
        printk(KERN_INFO "CEDRV: error in sending %d bytes\n",err_count);
    }
    return -EFAULT;
}

module_init(cedevdrv_init);
module_exit(cedevdrv_exit);
