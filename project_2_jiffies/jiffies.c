#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>

#define PROC_NAME "jiffies"

/* Function called when /proc/jiffies is read */
static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int rv = 0;
    char buffer[128];
    static int completed = 0;

    if (completed) {
        completed = 0;
        return 0;
    }

    completed = 1;
    /* Print current jiffies to buffer */
    rv = sprintf(buffer, "Current jiffies: %lu\n", jiffies);

    /* Copy kernel buffer to user space */
    if (copy_to_user(usr_buf, buffer, rv)) {
        return -EFAULT;
    }

    return rv;
}

/* Define proc_ops for modern kernels (5.6+) */
static const struct proc_ops jiffies_ops = {
    .proc_read = proc_read,
};

static int __init jiffies_init(void) {
    proc_create(PROC_NAME, 0666, NULL, &jiffies_ops);
    printk(KERN_INFO "Module jiffies loaded.\n");
    return 0;
}

static void __exit jiffies_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "Module jiffies removed.\n");
}

module_init(jiffies_init);
module_exit(jiffies_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module to report jiffies");
MODULE_AUTHOR("Trần Danh Quang");