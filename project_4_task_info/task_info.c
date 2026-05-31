#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/pid.h>

#define PROC_NAME "pid"

// Luu PID duoc ghi tu user
static long target_pid = -1;

// Xu ly khi doc /proc/pid, tra ve thong tin tien trinh theo PID
static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int rv = 0;
    char buffer[256];
    struct pid *pid_struct;
    struct task_struct *task;

    // Neu doc qua cuoi file hoac chua co PID thi tra ve 0 (EOF)
    if (*pos > 0 || target_pid == -1) {
        return 0;
    }

    // Lay struct pid tu PID integer
    pid_struct = find_vpid(target_pid);
    if (!pid_struct) {
        rv = sprintf(buffer, "Invalid PID: %ld\n", target_pid);
    } else {
        // Lay task_struct tu pid
        task = pid_task(pid_struct, PIDTYPE_PID);
        if (task) {
            // Kernel moi (>=5.14) dung __state thay vi state
            rv = sprintf(buffer, "command = [%s] pid = [%d] state = [%u]\n", 
                         task->comm, task->pid, task->__state);
        } else {
            rv = sprintf(buffer, "Task not found for PID: %ld\n", target_pid);
        }
    }

    // Copy buffer dinh dang ve user space
    if (copy_to_user(usr_buf, buffer, rv)) {
        return -EFAULT;
    }

    *pos = rv; // Cap nhat vi tri doc
    return rv;
}

// Xu ly khi ghi /proc/pid, nhan PID tu user space
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos) {
    char *k_mem;

    // Cap phat bo nho kernel
    k_mem = kmalloc(count + 1, GFP_KERNEL);
    if (!k_mem) {
        return -ENOMEM;
    }

    // Copy du lieu tu user space ve kernel space
    if (copy_from_user(k_mem, usr_buf, count)) {
        kfree(k_mem);
        return -EFAULT;
    }
    
    k_mem[count] = '\0'; // Dam bao ket thuc chuoi

    // Chuyen chuoi sang long va luu vao target_pid
    kstrtol(k_mem, 10, &target_pid);

    // Giai phong bo nho kernel
    kfree(k_mem);

    return count;
}

// Dung proc_ops cho kernel moi (>= 5.6) thay cho file_operations
static const struct proc_ops proc_fops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

// Khoi tao module
static int __init task_info_init(void) {
    // Tao entry /proc/pid cho doc/ghi (0666)
    proc_create(PROC_NAME, 0666, NULL, &proc_fops);
    printk(KERN_INFO "/proc/%s created successfully.\n", PROC_NAME);
    return 0;
}

// Don dep module
static void __exit task_info_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed successfully.\n", PROC_NAME);
}

module_init(task_info_init);
module_exit(task_info_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linux Kernel Module for Task Information");
MODULE_AUTHOR("TDQ");


