#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/moduleparam.h>

// Declare the parameter 
static int start = 25;
module_param(start, int, 0);

// Define the structure for the Collatz sequence 
struct collatz_node {
    int value;
    struct list_head list;
};

// Declare the list head 
static LIST_HEAD(collatz_list);

static int __init collatz_init(void) {
    int n = start;
    struct collatz_node *new_node, *ptr;

    printk(KERN_INFO "Loading Collatz Module with start = %d\n", start);

    if (n <= 0) {
        printk(KERN_WARNING "Start value must be greater than 0.\n");
        return -EINVAL;
    }

    // Generate the sequence and store in the linked list 
    while (1) {
        new_node = kmalloc(sizeof(*new_node), GFP_KERNEL);
        if (!new_node) return -ENOMEM;

        new_node->value = n;
        INIT_LIST_HEAD(&new_node->list);
        list_add_tail(&new_node->list, &collatz_list);

        if (n == 1) break;

        // Collatz conjecture rules 
        if (n % 2 == 0) {
            n = n / 2;
        } else {
            n = 3 * n + 1;
        }
    }

    // Traverse and print the list 
    printk(KERN_INFO "--- Collatz Sequence ---\n");
    list_for_each_entry(ptr, &collatz_list, list) {
        printk(KERN_INFO "%d\n", ptr->value);
    }
    printk(KERN_INFO "------------------------\n");

    return 0;
}

static void __exit collatz_exit(void) {
    struct collatz_node *ptr, *next;

    printk(KERN_INFO "Removing Collatz Module...\n");

    list_for_each_entry_safe(ptr, next, &collatz_list, list) {
        list_del(&ptr->list);
        kfree(ptr);
    }

    if (list_empty(&collatz_list)) {
        printk(KERN_INFO "Collatz list is now empty and memory is freed.\n");
    }
}

module_init(collatz_init);
module_exit(collatz_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel Data Structures - Part 2: Collatz Sequence");
MODULE_AUTHOR("TDQ");