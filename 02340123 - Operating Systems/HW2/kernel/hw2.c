#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/module.h>

// ===== Helping functions =====

#define TASKSTRUCT_K struct task_struct*

// ===== ======= ========= =====

asmlinkage long sys_hello(void) {
    printk("Hello, World!\n");
    return 0;
}

asmlinkage long sys_set_weight(int weight) {

    // Checking if the input is valid
    if (weight < 0)
        return -EINVAL;
    
    // Changing the weight of the current process
    current->weight = weight;
    return 0;

}

asmlinkage long sys_get_weight(void) {

    // Returning the weight of the current process
    return current->weight;

}

asmlinkage long sys_get_leaf_children_sum(void) {

    // Checking if the current process has any children
    if (list_empty(&current->children))
        return -ECHILD;

    // Making an array of all the processes
    long MaxSize = 100;
    long CurrentSize = 1;
    TASKSTRUCT_K* AllProcesses = kmalloc(sizeof(TASKSTRUCT_K) * MaxSize , GFP_KERNEL);
    AllProcesses[0] = current;
    
    // Iterating variables
    long CurrentIter = 0;
    long Total = 0;

    // =====
    struct list_head* wat;
    // =====

    // Iterating over every process and adding its children to the array
    while(CurrentIter != CurrentSize) {

        // Adding the weight of the current process if it has no children
        if (list_empty(&(AllProcesses[CurrentIter])->children))
            Total = Total + AllProcesses[CurrentIter]->weight;

        // Adding all of the children to our list
        list_for_each(wat,(&(AllProcesses[CurrentIter])->children)) {
            
            // Checking if there's enough space in the array and expanding if needed
            if(CurrentSize == MaxSize) {

                MaxSize = MaxSize * 2;
                TASKSTRUCT_K* TempArray = kmalloc(sizeof(TASKSTRUCT_K) * MaxSize , GFP_KERNEL);

                long i = 0;
                while (i < CurrentSize){

                    TempArray[i] = AllProcesses[i];
                    i++;

                }
                
                kfree(AllProcesses);
                AllProcesses = TempArray;

            }

            AllProcesses[CurrentSize] = list_entry(wat, struct task_struct, sibling);
            CurrentSize++;
            
        }

        // Advancing to the next process
        CurrentIter++;

    }
    
    // Deleting the allocated array
    kfree(AllProcesses);

    // Returning the total weight
    return Total;

}

asmlinkage long sys_get_heaviest_ancestor(void) {
    
    // Setting the Max weight & PID variables
    long MaxWeight = current->weight;
    long MaxPID = current->pid;

    // Going over all the parents of the current process
    struct task_struct* Iter = current;
    while (Iter->pid != 1) {
        
        // Getting the parent of the previous process
        Iter = Iter->real_parent;

        // Checking if the weight is higher & setting it as the max
        if (Iter->weight > MaxWeight) {
            MaxWeight = Iter->weight;
            MaxPID = Iter->pid;
        }

    }

    // Returning the PID of the process with the MaxWeight
    return MaxPID;
}