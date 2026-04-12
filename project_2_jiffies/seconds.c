#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <asm/param.h>

#define PROC_NAME "seconds"

static unsigned long start_jiffies; /* Store jiffies at load time */

static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int rv = 0;
    char buffer[128];
    static int completed = 0;
    unsigned long elapsed_seconds;

    if (completed) {
        completed = 0;
        return 0;
    }

    completed = 1;

    /* Calculation: (current_jiffies - start_jiffies) / HZ */
    elapsed_seconds = (jiffies - start_jiffies) / HZ;
    rv = sprintf(buffer, "Elapsed seconds: %lu\n", elapsed_seconds);

    if (copy_to_user(usr_buf, buffer, rv)) {
        return -EFAULT;
    }

    return rv;
}

static const struct proc_ops seconds_ops = {
    .proc_read = proc_read,
};

static int __init seconds_init(void) {
    start_jiffies = jiffies; /* Capture start time */
    proc_create(PROC_NAME, 0666, NULL, &seconds_ops);
    printk(KERN_INFO "Module seconds loaded.\n");
    return 0;
}

static void __exit seconds_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "Module seconds removed.\n");
}

module_init(seconds_init);
module_exit(seconds_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module to report elapsed seconds since loading");
MODULE_AUTHOR("Tran Danh Quang");