/////////////////////////////////////////////////////////////////////////////
// Name:         ceiofs.c
// Description:  IO sysfs device driver loadable kernel module
// Author:       Yan Naing Aye
// Date:         2020 Feb 13
// Ref: https://lwn.net/Kernel/LDD3/
/////////////////////////////////////////////////////////////////////////////

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/kobject.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/time.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cool Emerald");
MODULE_DESCRIPTION("GPIO driver");
MODULE_VERSION("1.0");

static unsigned int pinOut = 4;
static bool pulseEn = 0;
static spinlock_t tightLoop;

static ssize_t pulsing_store(struct kobject *kobj, struct kobj_attribute *attr,
    const char *buf, size_t count)
{
    unsigned int b;
    sscanf(buf,"%u",&b);
    pulseEn = b;
    return count;
}
    
static ssize_t pulsing_show(struct kobject *kobj, struct kobj_attribute *attr,
    char *buf)
{
    unsigned int b = pulseEn;
    return sprintf(buf,"%u\n",b);
}

static struct kobj_attribute pulsing_attr = __ATTR(pulsing,0664,pulsing_show,pulsing_store);

static struct attribute *pulser_attrs[] = {
 &pulsing_attr.attr,
 NULL
};

static struct attribute_group pulser_group = {
    .name = "pulser",
    .attrs = pulser_attrs,
};

static struct kobject *ceiofs_kobj;
static struct task_struct *task;

static int pulses_task(void *arg)
{
    unsigned long flags;
    ktime_t t0, tn;
    uint64_t intervalns;
    uint64_t pulsewidthns = 50000; // pulse width tick interval in ns
    int i=0;
    printk(KERN_INFO "CEIO: Thread has started.\n");
    while(!kthread_should_stop()){
        // enter high priority pulses loop
        spin_lock_irqsave(&tightLoop, flags);
        //udelay(50);
        t0 = ktime_get(); 
        i = 0;
        while(pulseEn){
            tn = ktime_get();
            intervalns = ktime_to_ns(ktime_sub(tn, t0));
            if( intervalns >= pulsewidthns ) {
                t0 = tn;
                gpio_set_value(pinOut,i%2);
                i++;                
            }
            if(i>=200) {
                pulseEn = 0;
                gpio_set_value(pinOut,0);
            }            
        }
        spin_unlock_irqrestore(&tightLoop, flags);
        // exit high priority mag pulses loop
        msleep(50);
    }
    printk(KERN_INFO "CEIO: Thread has stopped.\n");
    return 0;
}

static int __init ceiofs_init(void)
{
    int result = 0;
    printk(KERN_INFO "CEIO: init.\n");
    
    ceiofs_kobj = kobject_create_and_add("ceiofs",kernel_kobj->parent);
    if(!ceiofs_kobj){
        printk(KERN_ALERT "CEIO: error in creating kobject\n");
        return -ENOMEM;
    }
    result = sysfs_create_group(ceiofs_kobj,&pulser_group);
    if(result){
        printk(KERN_ALERT "CEIO: error in creating sysfs group\n");
        kobject_put(ceiofs_kobj);
        return result;
    }
    
    gpio_request(pinOut,"sysfs");
    gpio_direction_output(pinOut,false);
    gpio_export(pinOut,false);
    
    spin_lock_init(&tightLoop);
    
    task = kthread_run(pulses_task,NULL,"Pulsing_thread");
    if(IS_ERR(task)){
        printk(KERN_ALERT "CEIO: error in running thread\n");
        return PTR_ERR(task);
    }
    return result;
}

static void __exit ceiofs_exit(void)
{
    kthread_stop(task);
    kobject_put(ceiofs_kobj);
    gpio_set_value(pinOut,0);
    gpio_unexport(pinOut);
    gpio_free(pinOut);
    printk(KERN_INFO "CEIO: exit.\n");
}

module_init(ceiofs_init);
module_exit(ceiofs_exit);
