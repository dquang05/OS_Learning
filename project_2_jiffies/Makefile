# List of modules to build
obj-m += jiffies.o
obj-m += seconds.o

# Paths to your development environment
# KDIR points to the kernel source tree you compiled for ARM64
KDIR := /home/tdq/dev/linux
INSTALL_DIR := /home/tdq/dev/busybox/_install/lib/modules/

# Architecture and Toolchain
ARCH := arm64
CROSS_COMPILE := aarch64-linux-gnu-

# Local directory
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) KBUILD_MODPOST_WARN=1 modules

# Install target to move .ko files to BusyBox filesystem
install:
	mkdir -p $(INSTALL_DIR)
	cp jiffies.ko seconds.ko $(INSTALL_DIR)
	@echo "Modules installed to $(INSTALL_DIR)"

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean