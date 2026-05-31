#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>

// Define the structure with embedded list_head 
struct color {
    int red;
    int blue;
    int green;
    struct list_head list;
};

// Declare and initialize the head of the list 
static LIST_HEAD(color_list);

// Module initialization 
static int __init color_list_init(void) {
    struct color *violet, *cyan, *yellow, *black;
    struct color *ptr;

    printk(KERN_INFO "Loading Color List Module...\n");

    // 1. Allocate and initialize 4 colors 
    violet = kmalloc(sizeof(*violet), GFP_KERNEL);
    violet->red = 138; violet->green = 43; violet->blue = 226;
    INIT_LIST_HEAD(&violet->list);
    list_add_tail(&violet->list, &color_list);

    cyan = kmalloc(sizeof(*cyan), GFP_KERNEL);
    cyan->red = 0; cyan->green = 255; cyan->blue = 255;
    INIT_LIST_HEAD(&cyan->list);
    list_add_tail(&cyan->list, &color_list);

    yellow = kmalloc(sizeof(*yellow), GFP_KERNEL);
    yellow->red = 255; yellow->green = 255; yellow->blue = 0;
    INIT_LIST_HEAD(&yellow->list);
    list_add_tail(&yellow->list, &color_list);

    black = kmalloc(sizeof(*black), GFP_KERNEL);
    black->red = 0; black->green = 0; black->blue = 0;
    INIT_LIST_HEAD(&black->list);
    list_add_tail(&black->list, &color_list);

    // 2. Traverse the list and output to kernel log 
    printk(KERN_INFO "--- Current Colors in List ---\n");
    list_for_each_entry(ptr, &color_list, list) {
        printk(KERN_INFO "Color: R=%d, G=%d, B=%d\n", ptr->red, ptr->green, ptr->blue);
    }
    printk(KERN_INFO "------------------------------\n");

    return 0;
}

// Module cleanup 
static void __exit color_list_exit(void) {
    struct color *ptr, *next;

    printk(KERN_INFO "Removing Color List Module...\n");

    // 3. Safely remove elements and free memory 
    list_for_each_entry_safe(ptr, next, &color_list, list) {
        printk(KERN_INFO "Removing Color: R=%d, G=%d, B=%d\n", ptr->red, ptr->green, ptr->blue);
        list_del(&ptr->list);
        kfree(ptr);
    }

    // Check if the list is actually empty 
    if (list_empty(&color_list)) {
        printk(KERN_INFO "Color list is now empty. Memory freed successfully.\n");
    }
}

module_init(color_list_init);
module_exit(color_list_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel Data Structures - Part 1: Color List");
MODULE_AUTHOR("TDQ");