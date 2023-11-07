#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/gpio/consumer.h>
#include <linux/miscdevice.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define YELLOW_LED 23
#define LED 17

static char *KEYS_NAME = "PB_KEY";

int irqs;//[10];

bool led_blinking = false;
struct timer_list blink_timer;

size_t led_state = false;

static void toggle_led_state(unsigned long data)
{
    //struct device *dev = (struct device *)data;

    // Toggle the LED state
    led_blinking = !led_blinking;
    gpio_set_value(LED, led_blinking ? 1 : 0);

    // If still blinking, set the timer for the next toggle
    if (led_blinking)
        mod_timer(&blink_timer, jiffies + msecs_to_jiffies(500));  // Blink every 500ms
}

static irqreturn_t hello_keys_isr(int irq, void *data)
{
    struct device *dev = (struct device *)data;
    led_blinking = !led_blinking;
    if (led_blinking) {
        // Start blinking
        //led_blinking = true;
        timer_setup(&blink_timer, toggle_led_state, 0);
        mod_timer(&blink_timer, jiffies + msecs_to_jiffies(500));  // Blink every 500ms
    } else {
        // Stop blinking
        //led_blinking = false;
        del_timer_sync(&blink_timer);
        gpio_set_value(LED, 0);  // Turn off the LED
    }

    dev_info(dev, "Button pressed. LED blinking: %s\n", led_blinking ? "On" : "Off");

    return IRQ_HANDLED;

    /*gpio_set_value(RED_LED, led_state ? 1 : 0);

    return IRQ_HANDLED;*/
}


static ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *pos)
{
    size_t len;
    char kbuf[2000];

    if (*pos == 0) {
        kbuf[0] = '\0';
        //for (size_t i = 0; i < nb_devices; i++)
            //sprintf(kbuf, "%sid: %d: %lu\n", kbuf, irqs[i], irqs_count[i]);
        len = strlen(kbuf);
        if (copy_to_user(buf, kbuf, len))
            return -EFAULT;
        ++(*pos);
        return len;
    }
    return 0;
}


static ssize_t led_control_write(struct file *filp, const char *buf, size_t count, loff_t *offp)
{
    char command[10];

    if (copy_from_user(command, buf, min(count, sizeof(command))) != 0)
        return -EFAULT;

    if (strncmp(command, "start", 5) == 0) {
        if (!led_blinking) {
            led_blinking = true;
            timer_setup(&blink_timer, toggle_led_state, 0);
            mod_timer(&blink_timer, jiffies + msecs_to_jiffies(500));  // Start blinking every 500ms
        }
    } else if (strncmp(command, "stop", 4) == 0) {
        if (led_blinking) {
            led_blinking = false;
            del_timer_sync(&blink_timer);  // Stop blinking
            gpio_set_value(LED, 0);  // Turn off the LED
        }
    }

    //pr_info("%s --> %d\n", command, );

    return count;
}


static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = led_control_write,
};

static struct miscdevice my_miscdevice = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "led_control",
    .fops = &fops,
    .mode = 0666,
};

static int my_probe(struct platform_device *pdev)
{
    int ret_val, irq;
    struct device *dev = &pdev->dev;

    pr_info("my_probe enter\n");
    dev_info(dev, "my_probe() function is called.\n");
    irq = platform_get_irq(pdev, 0);
    if (irq < 0) {
        dev_err(dev, "irq is not available\n");
        return -EINVAL;
    }
    dev_info(dev, "IRQ_using_platform_get_irq: %d\n", irq);
    ret_val = devm_request_irq(dev, irq, hello_keys_isr, IRQF_SHARED, KEYS_NAME, dev);
    if (ret_val != 0) {
        dev_err(dev, "Error when request irq\n");
        return ret_val;
    }

    /*irqs[nb_devices] = irq;
    ++nb_devices;*/
    dev_info(dev, "my_probe() function is exited.\n");

    return 0;
}

static int  my_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "Remove called\n");
    del_timer(&blink_timer);
    gpio_set_value(LED, 0); 
    return 0;
}

static const struct of_device_id my_of_ids[] = {
    { .compatible = "training,intbutton" },
    {},
};

MODULE_DEVICE_TABLE(of, my_of_ids);
static struct platform_driver my_platform_driver = {
    .probe = my_probe,
    .remove = my_remove,
    .driver = {
        .name = "int_button",
        .of_match_table = my_of_ids,
        .owner = THIS_MODULE,
    }
};

static int __init my_init(void)
{
    int ret_val;

    pr_info("Push button driver enter\n");
    ret_val = platform_driver_register(&my_platform_driver);
    if (ret_val != 0) {
        pr_err("platform value returned %d\n", ret_val);
        return ret_val;
    }
    ret_val = misc_register(&my_miscdevice);
    if (ret_val != 0) {
        pr_err("misc register value returned %d\n", ret_val);
        return ret_val;
    }
    gpio_request(LED, "red");
    gpio_direction_output(LED, 1);

    gpio_request(YELLOW_LED, "yellow");
    gpio_direction_output(YELLOW_LED, 1);
    //led_state[0] = 1;
    //led_state[1] = 1;

    return 0;
}

static void __exit my_exit(void)
{
    pr_info("Push button driver exit\n");
    gpio_set_value(LED, 0);
    gpio_set_value(YELLOW_LED, 0);
    gpio_free(LED);
    gpio_free(YELLOW_LED);
    misc_deregister(&my_miscdevice);
    platform_driver_unregister(&my_platform_driver);
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kemoko KEITA");
MODULE_DESCRIPTION("Using interrupts with a push button");

