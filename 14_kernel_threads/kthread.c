#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h> /* struct task_struct for threads */
#include <linux/delay.h>

/* global variables for the thread */
static struct task_struct *kthread_1;
static struct task_struct *kthread_2;
static int t1 = 1, t2 = 2;

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("kernel threads in LKM");

/** 
 * @brief kthread function which will be executed by the threads
 * 
 * @param arg pointer to the thread number
 */
int thread_fn(void *arg)
{
    unsigned int i = 0;
    int thread_nr = *(int *)arg; // dereference arg

    /* working loop */
    while (!kthread_should_stop()) {
        printk(KERN_INFO "kthread - Thread %d is executed, counter val: %d\n", 
                thread_nr, i++);
        msleep(thread_nr * 1000);
    }

    printk(KERN_INFO "kthread - Thread %d finished execution\n", thread_nr);
    return 0;
}


/**
 * @brief This function is called when the module is loaded into the kernel
 */
static int __init module_init_func(void)
{
    printk(KERN_INFO "Kernel module loaded\n");
    printk(KERN_INFO "kthread - Init threads\n");

    /* for kthread_1: kthread_create() + wake_up_process() */
    kthread_1 = kthread_create(thread_fn, &t1, "kthread_1");
    if(kthread_1 != NULL) {
        /* Let's start kthread_1 */
        wake_up_process(kthread_1);
        printk(KERN_INFO "kthread - Thread 1 has been created and is running now\n");
    }
    else {
        printk(KERN_INFO "kthread - Thread 1 cannot be created\n");
        return -1;
    }
    
    /* let's use another way for kthread_2: kthread_run() */
    kthread_2 = kthread_run(thread_fn, &t2, "kthread_2");
    if(kthread_2 != NULL) {
        printk(KERN_INFO "kthread - Thread 2 has been created and is running now\n");
    }
    else {
        printk(KERN_INFO "kthread - Thread 2 cannot be created\n");
        kthread_stop(kthread_1); /* we want both thread 1 and 2 running */
        return -1;
    }

    printk(KERN_INFO "kthread - Both threads are running now\n");
    msleep(5000);
    printk(KERN_INFO "kthread - after sleeping for 5 sec, close two threads\n");
    kthread_stop(kthread_1);
    kthread_stop(kthread_2);
    return 0;
}


/**
 * @brief This function is called when the module is removed from the kernel
 */
static void __exit module_exit_func(void)
{
    printk(KERN_INFO "Kernel module unloaded\n");
    printk(KERN_INFO "kthread - Stop both threads\n");
}


module_init(module_init_func);
module_exit(module_exit_func);
