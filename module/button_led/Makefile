obj-m := button_led.o ../dht11/dht11.o
KERNEL_DIR ?= $(HOME)/Documents/formation/POEI/drivers_linux/rpi3b64/linux
#DHT11_MODULE_DIR := 

all:
	make -C $(KERNEL_DIR) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=$(PWD) clean

deploy:
	scp *.ko kmk@172.20.10.2:/home/kmk
