obj-m += task_info.o

# Đường dẫn tới Kernel Source của bạn
KDIR := /home/tdq/dev/linux
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules

install:
	cp task_info.ko /home/tdq/dev/busybox/_install/

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean