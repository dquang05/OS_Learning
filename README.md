# General Guide for Linux Kernel Module Development (ARM64)

This document outlines my workflow for developing, cross-compiling, and deploying Out-of-Tree Kernel Modules for an ARM64 target using QEMU and BusyBox.

---

## Phase 1: Environment Setup (The Foundation)

### 1. Configure Cross-Toolchain
* **Step:** Export `ARCH=arm64` and `CROSS_COMPILE=aarch64-linux-gnu-`.
* **Why:** Your Host (Ubuntu) is likely x86_64, but the Target (QEMU/Quang-OS) is ARM64. You must use a compiler that generates ARM64 machine code.
* **Error if skipped:** `Exec format error` or `Invalid module format` when running `insmod`.

### 2. Prepare Kernel Tree (The Symbol Map)
* **Step:** Run `make modules_prepare` in the Linux kernel source directory.
* **Why:** Modules need to know the memory addresses of Kernel functions (e.g., `printk`, `jiffies`). This generates the `Module.symvers` file, which acts as a "phone book" for symbols.
* **Error if skipped:** `WARNING: "symbol" undefined!` during compilation and `Unknown symbol` error during `insmod`.

---

## Phase 2: RootFS Preparation (The Storage)

### 3. Create System Mount Points
* **Step:** `mkdir -p proc sys dev etc lib/modules` in the BusyBox `_install` directory.
* **Why:** Linux uses virtual filesystems (`procfs`, `sysfs`) to communicate between Kernel and User space. These directories must exist as "anchor points" for the Kernel to mount them.
* **Error if skipped:** `mount: mounting proc on /proc failed: No such file or directory`.

### 4. Setup Auto-Mount (Optional but Recommended)
* **Step:** Create `/etc/init.d/rcS` containing `mount -t proc none /proc` and `mount -t sysfs none /sys`.
* **Why:** Automates the setup so you don't have to manually mount filesystems every time the OS boots.
* **Error if skipped:** You will see `No such file or directory` when trying to access `/proc/` entries or running `lsmod`.

---

## Phase 3: Development Cycle (The Loop)

### 5. Compile & Install Module
* **Step:** `make` then `make install` (copying `.ko` to `_install/lib/modules`).
* **Why:** Compiles your C code into a Kernel Object (`.ko`) and places it where BusyBox can find it.
* **Error if skipped:** `insmod: can't open 'module.ko': No such file or directory`.

### 6. Repackage RootFS
* **Step:** Re-run the `cpio` command to update `rootfs.cpio.gz`.
* **Why:** QEMU loads the filesystem into RAM at boot. If you don't repackage, the new `.ko` files you just compiled won't exist inside the virtual machine.
* **Error if skipped:** Old filesystem remains; new modules are missing inside QEMU.

---

## Phase 4: Runtime Execution (The Target)

### 7. Load Module
* **Step:** `insmod <module>.ko`.
* **Why:** Dynamically links your code into the running Kernel.
* **Error if skipped:** `Operation not permitted` (if not root) or `Module already exists`.

### 8. Debugging
* **Step:** Use `dmesg` to check Kernel logs.
* **Why:** Since Kernel modules don't have a standard console, `printk` outputs are sent to a ring buffer. `dmesg` is the only way to see what went wrong.

---

## Summary Table: Troublehooting Reference

| Symptom | Likely Cause | Solution |
| :--- | :--- | :--- |
| `undefined!` symbols in build | Missing `Module.symvers` | Build Kernel / `make modules_prepare` |
| `Unknown symbol` on `insmod` | Kernel/Module mismatch | Ensure `KDIR` matches running Kernel |
| `/proc/` file not found | Procfs not mounted | `mount -t proc none /proc` |
| `Exec format error` | Architecture mismatch | Check `ARCH` and `CROSS_COMPILE` |