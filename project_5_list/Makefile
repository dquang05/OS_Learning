# Khai báo build cả 2 module
obj-m += list.o collatz.o

KDIR := /home/tdq/dev/linux
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules

install:
	cp list.ko /home/tdq/dev/busybox/_install/
	cp collatz.ko /home/tdq/dev/busybox/_install/

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean