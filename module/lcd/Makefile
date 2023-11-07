obj-m := lcd.o ../dht11/dht11.o
KERNEL_DIR ?= $(HOME)/Documents/formation/POEI/drivers_linux/rpi3b64/linux

all:
	make -C $(KERNEL_DIR) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=$(PWD) clean

deploy:
	scp *.ko kmk@192.168.1.46:/home/kmk
