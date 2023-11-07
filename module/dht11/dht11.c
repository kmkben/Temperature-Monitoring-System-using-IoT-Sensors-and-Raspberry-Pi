#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/miscdevice.h>
#include <linux/of.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/sysfs.h>
#include <linux/init.h>

#define DHT11_DATA_PIN 4 

static int dht11_pin;// = DHT11_DATA_PIN;

static u8 humidity = 0;
u8 temperature = 0;
EXPORT_SYMBOL(temperature);

// Function to send start signal to DHT11
static void SendStartSignal(void) {
    // Set data pin as output
    gpio_direction_output(dht11_pin, 0);
    //udelay(20000); // 20ms delay with data pin low
    msleep(20);
    gpio_set_value(dht11_pin, 1);
    //gpio_direction_output(dht11_pin, 1);
    gpio_direction_input(dht11_pin);
    //udelay(30); // Delay after pulling high
}

// Function to wait for low signal
static int WaitForLow(void) {
    // Wait for data pin to go low and return time in microseconds
    
    ktime_t start_time, end_time, duration;
    s64 microseconds;
    start_time = ktime_get();
    do
    {
        end_time = ktime_get();
        duration = ktime_sub(end_time, start_time);
        microseconds = ktime_to_us(duration);
    }while(gpio_get_value(dht11_pin)==1 && microseconds < 1000);

    if(microseconds >= 1000)
        pr_info("timeout WaitForLow\n");

    return microseconds;
}

// Function to wait for high signal
static int WaitForHigh(void) {
    // Wait for data pin to go high and return time in microseconds
    
    ktime_t start_time, end_time, duration;
    s64 microseconds;
    start_time = ktime_get();
    
    do{
        end_time = ktime_get();
        duration = ktime_sub(end_time, start_time);
        microseconds = ktime_to_us(duration);
    }while(gpio_get_value(dht11_pin)==0 && microseconds < 1000); 
  
    if(microseconds >= 1000)
        pr_info("timeout WaitForHigh\n");
  
    return microseconds;

    //return 0;
}

// Function to calculate parity
static u8 CalculateParity(u8 HumidityHigh, u8 HumidityLow, u8 TemperatureHigh, u8 TemperatureLow) {
    return (HumidityHigh + HumidityLow + TemperatureHigh + TemperatureLow);
}

// Function to process measured data
static void ProcessData(u64 Data) {
    // Extract temperature and humidity from Data and store in global variables

    u8 humidity_high = (Data >> 32) & 0xFF;
    u8 humidity_low = (Data >> 24) & 0xFF;
    u8 temperature_high = (Data >> 16) & 0xFF;
    u8 temperature_low = (Data >> 8) & 0xFF;
    u8 parity = Data & 0xFF;
    
    if (CalculateParity(humidity_high, humidity_low, temperature_high, temperature_low) == parity)
    {
        humidity = humidity_high;
        temperature = temperature_high;

        pr_info("h = %d and t = %d\n", humidity_high, temperature_high);
    }
    else
    {
        pr_err("Error parity\n");
        pr_err("Error parity, HumidityHigh:%d, HumidityLow:%d, TemperatureHigh:%d, TemperatureLow:%d, Parity:%d \n"
        , humidity_high, humidity_low, temperature_high, temperature_low, parity);
    }
}



// Function to measure data from DHT11
static void Measure(void) {
    
    u64 bits_result = 0;
    s64 low, high;

    SendStartSignal();
    // Implement data measurement process using WaitForLow, WaitForHigh, etc.
    // Extract raw data and call ProcessData

    WaitForLow();
    WaitForHigh();
    WaitForLow();

    /*u8 humidityHigh = 0;
    u8 humidityLow = 0;
    u8 temperatureHigh = 0;
    u8 temperatureLow = 0;
    u8 parity = 0;

    parity = CalculateParity(humidityHigh, humidityLow, temperatureHigh, temperatureLow);

    if (parity)
    {
        ProcessData((u64)humidityHigh << 24 | (u64)humidityLow << 16 | (u64)temperatureHigh << 8 | temperatureLow);
    }*/

    //u64 bits_result = 0;
    //s64 low, high;

    for (int i = 0; i < 40; ++i)
    {
        bits_result <<= 1;
        //bits_result = (WaitForLow() > WaitForHigh()) ? bits_result : bits_result | 1;
        low = WaitForHigh();
        high = WaitForLow();

        if (high > low)
        {
            bits_result |= 1;
        }
    }

    WaitForHigh();
    gpio_direction_output(dht11_pin, 1);

    ProcessData(bits_result);

}

/*
 * Export Temperature in order to be access in other module
 *
 */

//Create sysfs attribute functions to read the temperature:
/*static ssize_t temperature_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", temperature);
}

static struct kobj_attribute temperature_attr = __ATTR_RO(temperature);

//Define a sysfs directory and add the temperature attribute to it
static struct attribute *attrs[] = {
    &temperature_attr.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *my_kobj;

*
 * End Exporting temperature
 */

static int dht11_open(struct inode *inode, struct file *filp)
{
    return 0;
}

static int dht11_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t dht11_read(struct file *filp, char *buf, size_t count, loff_t *offp)
{
    char data[256];
    size_t data_len;
    int ret;

    Measure();

    if (*offp == 0)
    {
        *offp = 1;
        //Measure();
        //ProcessData(data);

        sprintf(data, "%d\n", temperature);
        data_len = strlen(data);


        // Copy data from kernel space to user space
        ret = copy_to_user(buf, data, data_len);
        if (ret != 0)
            return 0; // Error copying data to user space

        // Update the file position and return the number of bytes read

        pr_info("DHT11 reading ...\n");

        return data_len;
    }

    return 0;
}

static ssize_t dht11_write(struct file *filp, const char *buf, size_t count, loff_t *offp)
{
    pr_info("DHT11 writting ...\n");
    return 0;
}

static int dht11_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    pr_info("DHT11 probe function called ...\n");
    dev_info(dev, "my_dth11_probe() function is called.\n");
    //dev_info(pdev->dev, "DHT11 probe() function is called.\n");
    of_property_read_u32(pdev->dev.of_node,"pin_dht11", &dht11_pin);

    gpio_request(dht11_pin, "dht11 Pin");
    gpio_direction_input(dht11_pin);
    gpio_direction_output(dht11_pin,1);

    pr_info("dht11 pin --> %d\n", dht11_pin);

    return 0;
}

static int  dht11_remove(struct platform_device *pdev) 
{
    gpio_free(dht11_pin);
    return 0;
}

static const struct file_operations dht11_fops = {
    .owner = THIS_MODULE,
    .open = dht11_open,
    .release = dht11_release,
    .read = dht11_read,
    .write = dht11_write,
};

static const struct of_device_id dht11_of_ids[] = {
        { .compatible = "training,dht11"},
        {},
};

MODULE_DEVICE_TABLE(of, dht11_of_ids);

static struct miscdevice my_miscdevice = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = "dht11",
        .mode = 0666,
        .fops = &dht11_fops,
};


static struct platform_driver dht11_platform_driver = {
        .probe = dht11_probe,
        .remove = dht11_remove,
        .driver = {
                .name = "dht11_h_temp",
                .of_match_table = dht11_of_ids,
                .owner = THIS_MODULE,
        }
};

static int __init dht11_driver_init(void) 
{
    int ret_val;

    ret_val = platform_driver_register(&dht11_platform_driver);
    if (ret_val !=0)
    {
        pr_err("platform dht11 value returned %d\n", ret_val);
        return ret_val;
    }


    pr_info("Misc registering...\n");
    ret_val = misc_register(&my_miscdevice);
    if (ret_val != 0) {
            pr_err("could not register the misc device mydev\n");
            return ret_val;
    }

    // Initialize GPIO and other necessary components
    //gpio_request(dht11_pin, "DHT11 Data Pin");
    //gpio_direction_output(dht11_pin, 1);

    // Initialize other necessary initializations

    /*my_kobj = kobject_create_and_add("dht11", kernel_kobj);
    if (!my_kobj)
    {
        pr_err("Failed to create sysfs directory\n");
        return -ENOMEM;
    }

    // Create the sysfs attribute group
    if (sysfs_create_group(my_kobj, &attr_group) < 0)
    {
        pr_err("Failed to create sysfs group\n");
        kobject_put(my_kobj);
        return -ENOMEM;
    }*/

    printk(KERN_INFO "DHT11 Driver initialized\n");
    return 0;
}

static void __exit dht11_driver_exit(void) {
    // Free GPIO and other resources
    //gpio_free(dht11_pin);

    // Perform other cleanup
    

    misc_deregister(&my_miscdevice);
    platform_driver_unregister(&dht11_platform_driver);

    printk(KERN_INFO "DHT11 Driver exited\n");
}

module_init(dht11_driver_init);
module_exit(dht11_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kemoko KEITA");
MODULE_DESCRIPTION("DHT11 Temperature and Humidity Sensor Driver");
