obj-m += simple.o

KDIR := /home/tdq/dev/linux
# Path
INSTALL_DIR := /home/tdq/dev/busybox/_install/lib/modules/

ARCH := arm64
CROSS_COMPILE := aarch64-linux-gnu-

all:
	make -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) KBUILD_MODPOST_WARN=1 modules

# Auto install the module to the target directory
install:
	mkdir -p $(INSTALL_DIR)
	cp simple.ko $(INSTALL_DIR)

clean:
	make -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean