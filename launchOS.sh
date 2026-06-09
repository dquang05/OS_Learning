#!/bin/bash
set -e

# Get the project directory name from the first argument
PROJECT_DIR=$1

# Check if the user provided a project name
if [ -z "$PROJECT_DIR" ]; then
  echo "ERROR: Project name is missing!"
  echo "Usage: ./launchOS.sh <project_directory_name>"
  echo "Example: ./launchOS.sh project_3_unix_shell"
  exit 1
fi

PROJECT_PATH="/home/tdq/dev/OS_Learning/$PROJECT_DIR"
ROOTFS_BIN="/home/tdq/dev/busybox/_install/bin"

echo " === START BUILD & BOOT OS === "
echo "Processing project: $PROJECT_DIR"

# 1. Auto-detect compilation method
echo "[1/4] Analyzing compilation method..."
cd $PROJECT_PATH

# Check for Makefile to determine if it's a Kernel Module or User Space App
if [ -f "Makefile" ] || [ -f "makefile" ]; then
    echo "  >>> MAKEFILE FOUND: Compiling as a Kernel Module <<<"
    make
    make install
else
    echo "  >>> NO MAKEFILE: Compiling as a User Space App <<<"
    
    # Use cross-compiler to statically compile all .c files
    # Set the executable name to match the project directory name
    EXECUTABLE_NAME=$PROJECT_DIR
    aarch64-linux-gnu-gcc -static -Wall -o $EXECUTABLE_NAME *.c
    
    # Copy the executable to RootFS
    cp $EXECUTABLE_NAME $ROOTFS_BIN/
    echo "  -> Copied executable '$EXECUTABLE_NAME' to RootFS /bin/"
fi

# 2. Pack RootFS (initramfs)
echo "[2/4] Compiling RootFS (initramfs)..."
cd /home/tdq/dev/busybox/_install
find . | cpio -o --format=newc | gzip > /home/tdq/dev/busybox/rootfs.cpio.gz

# 3. Init QEMU
echo "[3/4] Init QEMU..."
echo "========================================="
echo "   >>> PRESS ENTER TO ACTIVATE CONSOLE (~ #) <<<"
if [ ! -f "$PROJECT_PATH/Makefile" ]; then
    echo "   >>> Type command: '$PROJECT_DIR' to run app! <<<"
fi
echo "========================================="
sleep 2

qemu-system-aarch64 -M virt -cpu cortex-a57 -nographic -smp 2 -m 1024M \
  -kernel /home/tdq/dev/linux/arch/arm64/boot/Image \
  -initrd /home/tdq/dev/busybox/rootfs.cpio.gz \
  -append "console=ttyAMA0 root=/dev/ram rdinit=/sbin/init"