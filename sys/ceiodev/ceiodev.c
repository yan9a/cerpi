/////////////////////////////////////////////////////////////////////////////
// Name:         ceiodev.c
// Description:  IO device driver loadable kernel module
// Author:       Yan Naing Aye
// Date:         2020 Feb 13
// Ref: https://lwn.net/Kernel/LDD3/
/////////////////////////////////////////////////////////////////////////////

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cool Emerald");
MODULE_DESCRIPTION("GPIO driver for RPi");
MODULE_VERSION("1.0");

static unsigned int pinOut = 17; // GPIO 17 which is pin 11 on the header
static unsigned int pinIn = 27; // GPIO 27 which is pin 13 on the header
static unsigned int irqno;
static irq_handler_t pininput_handler(unsigned int irq, void *dev_id,
    struct pt_regs *regs);

static int __init ceiodev_init(void)
{
  int irqreqresult = 0;
  printk(KERN_INFO "CEIO: init\n");
  
  if(!gpio_is_valid(pinOut)){
      printk(KERN_INFO "CEIO: invalid pinOut\n");
      return -ENODEV;
  }
  gpio_request(pinOut,"sysfs");
  gpio_direction_output(pinOut,1); // set dir out and set it 1 (high)
  gpio_export(pinOut,false); // export to appear as /sys/class/gpio/gpio17
  // false to disable changing the direction

  if(!gpio_is_valid(pinIn)){
      printk(KERN_INFO "CEIO: invalid pinIn\n");
      return -ENODEV;
  }
  gpio_request(pinIn,"sysfs");
  gpio_direction_input(pinIn); // set dir input
  gpio_export(pinIn,false); // export to appear as /sys/class/gpio/gpio27
  
  irqno = gpio_to_irq(pinIn);
  printk(KERN_INFO "CEIO: irq no: %d\n",irqno);
  irqreqresult = request_irq(irqno,(irq_handler_t)pininput_handler,
      IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,"ceio_pin_handler",NULL); 
      // ceio_pin_handler will appear as owner identity in /proc/interrupts
  printk(KERN_INFO "CEIO: irq result: %d\n",irqreqresult);
  return 0;
}

static void __exit ceiodev_exit(void)
{
  gpio_set_value(pinOut,0);
  gpio_unexport(pinOut);
  gpio_free(pinOut);
  
  free_irq(irqno,NULL);
  gpio_unexport(pinIn);
  gpio_free(pinIn);
  printk(KERN_INFO "CEIO: exit\n");
}

static irq_handler_t pininput_handler(unsigned int irq, void *dev_id,
    struct pt_regs *regs)
{
    int s = gpio_get_value(pinIn);
    gpio_set_value(pinOut,s);
    
    printk(KERN_INFO "CEIO: pin input: %d\n",s);
    return (irq_handler_t)IRQ_HANDLED;    
}

module_init(ceiodev_init);
module_exit(ceiodev_exit);
