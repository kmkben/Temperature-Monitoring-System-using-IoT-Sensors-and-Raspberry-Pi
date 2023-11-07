#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/gpio/consumer.h>
#include <linux/miscdevice.h>
#include <linux/of.h>
#include <linux/gpio.h>

#define LED 17 

static char *KEYS_NAME = "PB_KEY";

int irq;
size_t button_press_count = 0;
bool led_state = false;  // Initially off

static irqreturn_t button_isr(int irq, void *data)
{
    struct device *dev = data;

    button_press_count++;
    led_state = !led_state;  // Toggle LED state

    dev_info(dev, "Button pressed. LED state: %s\n", led_state ? "On" : "Off");

    // Set the LED state based on the button press
    gpio_set_value(LED, led_state ? 1 : 0);

    return IRQ_HANDLED;
}

static ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *pos)
{
    size_t len;
    char kbuf[200];

    if (*pos == 0) {
        snprintf(kbuf, sizeof(kbuf), "Button press count: %lu\n", button_press_count);
        len = strlen(kbuf);
        if (copy_to_user(buf, kbuf, len))
            return -EFAULT;
        ++(*pos);
        return len;
    }
    return 0;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
};

static struct miscdevice my_miscdevice = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "button_led",
    .fops = &fops,
    .mode = 0666,
};

static int my_probe(struct platform_device *pdev)
{
    int ret_val;
    struct device *dev = &pdev->dev;

    pr_info("my_probe enter\n");
    dev_info(dev, "my_probe() function is called.\n");
    irq = platform_get_irq(pdev, 0);
    if (irq < 0) {
        dev_err(dev, "irq is not available\n");
        return -EINVAL;
    }
    dev_info(dev, "IRQ_using_platform_get_irq: %d\n", irq);
    ret_val = devm_request_irq(dev, irq, button_isr, IRQF_SHARED, KEYS_NAME, dev);
    if (ret_val != 0) {
        dev_err(dev, "Error when request irq\n");
        return ret_val;
    }
    dev_info(dev, "my_probe() function is exited.\n");

    return 0;
}

static int  my_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "Remove called\n");
    return 0;
}

static const struct of_device_id my_of_ids[] = {
    { .compatible = "training,intbutton"},
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
    gpio_request(LED, "Red");
    gpio_direction_output(LED, 0);  // Initially off

    return 0;
}

static void __exit my_exit(void)
{
    pr_info("Push button driver exit\n");
    gpio_set_value(LED, 0);
    gpio_free(LED);
    misc_deregister(&my_miscdevice);
    platform_driver_unregister(&my_platform_driver);
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kemoko KEITA");
MODULE_DESCRIPTION("Toggle LED state with a push button");

