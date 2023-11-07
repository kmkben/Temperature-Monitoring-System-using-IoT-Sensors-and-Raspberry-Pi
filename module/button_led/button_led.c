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

extern u8 temperature;

int irqs;
bool led_blinking = false;
struct timer_list blink_timer;
static struct timer_list temperature_check_timer;

static ssize_t led_control_write(struct file *filp, const char *buf, size_t count, loff_t *offp);

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = led_control_write,
};

static struct miscdevice my_miscdevice = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "led_control",
    .fops = &fops,
    .mode = 0666,
};

static void toggle_led_state(struct timer_list *unused)
{
    //led_blinking = !led_blinking;
    //gpio_set_value(LED, led_blinking ? 1 : 0);
    gpio_set_value(LED, !gpio_get_value(LED));

    //if (led_blinking)
    mod_timer(&blink_timer, jiffies + msecs_to_jiffies(500));  // Blink every 500ms
}

static void check_temperature_and_blink(struct timer_list *unused)
{
    if (temperature >= 25) {
        // Start blinking the LED
        led_blinking = true;
        timer_setup(&blink_timer, toggle_led_state, 0);
        mod_timer(&blink_timer, jiffies + msecs_to_jiffies(500));  // Start blinking every 500ms
        gpio_set_value(LED, 1);  // Turn on the LED
    } else {
        // Stop blinking and turn off the LED
        if (led_blinking) {
            led_blinking = false;
            del_timer_sync(&blink_timer);  // Stop blinking
            gpio_set_value(LED, 0);  // Turn off the LED
        }
    }

    // Reschedule the timer for the next temperature check
    mod_timer(&temperature_check_timer, jiffies + msecs_to_jiffies(60000));  // Check temperature every 60 seconds
}

static irqreturn_t hello_keys_isr(int irq, void *data)
{
    struct device *dev = (struct device *)data;
    
    //led_blinking = !led_blinking;

    if (!led_blinking) {
        // Start blinkinga
        led_blinking = true;
        timer_setup(&blink_timer, toggle_led_state, 0);
        mod_timer(&blink_timer, jiffies + msecs_to_jiffies(500));  // Blink every 500ms
    } else {
        // Stop blinking
        led_blinking = false;
        del_timer_sync(&blink_timer);
        gpio_set_value(LED, 0);  // Turn off the LED
    }

    dev_info(dev, "Button pressed. LED blinking: %s\n", led_blinking ? "On" : "Off");

    //led_state = !led_state;  // Toggle LED state

    //dev_info(dev, "Button pressed. LED state: %s\n", led_state ? "On" : "Off");

    // Set the LED state based on the button press
    //gpio_set_value(LED, led_state ? 1 : 0);


    return IRQ_HANDLED;
}

// Initialization function for the temperature reading timer
static void init_temperature_timer(void)
{
    // Initialize and start the temperature check timer
    timer_setup(&temperature_check_timer, check_temperature_and_blink, 0);
    mod_timer(&temperature_check_timer, jiffies + msecs_to_jiffies(60000));
}

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
        dev_err(dev, "Error when requesting irq\n");
        return ret_val;
    }

    irqs = irq;  // Store the IRQ

    dev_info(dev, "my_probe() function is exited.\n");

    return 0;
}

static int  my_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "Remove called\n");
    del_timer_sync(&blink_timer);
    gpio_set_value(LED, 0);
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

    return count;
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
    gpio_set_value(LED, 0);
    gpio_direction_output(LED, 0);

    gpio_request(YELLOW_LED, "yellow");
    gpio_direction_output(YELLOW_LED, 0);

    //init_temperature_timer();

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

    // Clean up the temperature reading timer
    //del_timer_sync(&temperature_check_timer);
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kemoko KEITA");
MODULE_DESCRIPTION("Using interrupts with a push button");

