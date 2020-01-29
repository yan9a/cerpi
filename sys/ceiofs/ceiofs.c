#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/kobject.h>
#include <linux/kthread.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cool Emerald");
MODULE_DESCRIPTION("GPIO driver");
MODULE_VERSION("1.0");

static unsigned int pinOut = 4;
static bool pulseEn = 0;

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
    int i=0;
    printk(KERN_INFO "CEIO: Thread has started.\n");
    while(!kthread_should_stop()){
        set_current_state(TASK_RUNNING);
        if(pulseEn){
            for(i=0;i<200;i++){
                gpio_set_value(pinOut,i%2);
                udelay(50);
            }
            pulseEn = 0;
            gpio_set_value(pinOut,0);
        }
        set_current_state(TASK_INTERRUPTIBLE);
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
