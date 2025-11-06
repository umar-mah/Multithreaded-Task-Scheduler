# Task Scheduler

This repository contains a **Task Scheduler** system designed for managing tasks in both **user-space** and **kernel-space**. It includes a user-space implementation and a kernel-space that uses system calls to add, schedule, and manage tasks. The scheduler supports high, medium, and low-priority tasks, and it ensures resource allocation and task execution based on priority and resource availability. 

## Overview

### File Structure
- User-space files are pushed into the master branch.
- Kernel-space files are pushed into the main branch.
- Please switch between the branches to run the code.

### User-Space
- **User-space program** uses pthreads to create an emulation of the kernel version
- Allows for adding tasks with specific duration, priorities and resources, scheduling tasks, and managing task execution.
- Tasks are placed in queues based on priority: high, medium, or low.
- Provides an interface to request resources for tasks and handle task completion.
- Resources are managed with semaphores to ensure mutual exclusion and prevent resource conflicts.
I implemented **two semaphore locks** for scheduling: one for the **main scheduler** and another for the **waiting scheduler** to ensure proper synchronization and prevent race conditions.
- The scheduler handles the execution of tasks based on their priority and resource availability.
- Built a task consumer and task producer function. Task producer function creates a task and connects them with a producer thread.
- Consumer threads execute the tasks by acquiring resources. Each consumer thread works independently. Semaphores are used to manage the resources and task access.
- In addition, there is a waiting scheduler as well which executes tasks in the waiting queue.
- Waiting queue holds tasks that could not be executed due to the lack of resources. Once resources are available, they execute.  

The **main functions** include:  
- **Enqueue**: Adds tasks to the appropriate **priority queue** based on their priority level.  
- **Dequeue**: Removes tasks from the **priority queue** for execution.  
- **Acquire resources**: Allocates required resources for a task. Each resource has a **semaphore lock**, ensuring that only one task can access it at a time.  
- **Release resources**: Frees the allocated resources and releases the **semaphore lock** for other tasks to use.
 
The **Functions Used for Testing** include:  
- **Task Producer:**  
  - Generates random tasks with a **random ID, duration, priority, and required resources**.  
  - Uses the **enqueue function** to add tasks to the **high, medium, or low priority queues** based on their priority level.  
- **Task Consumer:**  
  - Acts as the **main scheduler**, executing tasks by **dequeuing** them from the priority queues.  
  - Acquires resources before execution. If resources are unavailable, the task is placed in the **waiting queue**.  
  - The **waiting queue** executes tasks as soon as the required resources become available.  
- **Queue Initialization & Cleanup:**  
  - All **queues are initialized** properly using dedicated functions.  
  - Functions are executed in `main` to manage task scheduling.  
  - The **clean scheduler function** ensures that all allocated memory is freed, preventing **memory leaks**.

#### Testing
- I tested all the functions by creating a **task producer** and a **task consumer**. I conducted tests with **three consumers** and **multiple producers** (eight and six tasks, respectively).  
- The project compiled successfully **without errors**, and valgrind testing confirmed that there are **no memory leaks**.
- To compile program, run `make` and then `make run`.

### Kernel-Space
- **Kernel module** implements the task scheduler, handling task management, resource allocation, and scheduling.
- It is suppose to do the same thing as the user space programs.
- It has the same functions but slightly different implementations than user-space.

Uses **System Calls**:  
The kernel module provides several system calls to interact with the scheduler and manage tasks:
1. **`add_task`**: Adds a task to the appropriate priority queue (high, medium, low) with the specified parameters.
2. **`schedule_task`**: Starts task scheduling by dequeuing tasks from the priority queues and attempting resource allocation.
3. **`acquire_resources`**: Allows a task to acquire resources for execution.
4. **`release_resources`**: Releases resources once a task has completed.
5. **`init_scheduler`**: Initializes the task scheduler and queues.
6. **`exit_scheduler`**: Cleans up the scheduler, freeing resources and tasks.

#### Differences 
- `kqueue` is implemented using **`list_head`**, which functions as a **linked list** for task management.  
- **Spinlocks** are used instead of **mutex locks** to ensure efficient synchronization in the kernel.  
- Includes **`#include <linux/semaphore.h>`** for **semaphore-based resource management**.  
- Uses **`down_trylock()`** to acquire resources and **`up()`** to release them.  
- **`printk(KERN_INFO "...")`** is used for debugging and tracking execution.  
- The command **`dmesg -T | tail -50`** can be used to view the last 50 kernel logs.  
- **One spinlock for the scheduler** to manage task execution.  
- **Another spinlock for resources** to ensure atomic access to shared resources.  
- Uses **`list_for_each_entry_safe()`** to **iterate and free all tasks** from the queues during cleanup.
- Uses kthreads in the kernel instead of pthreads in main.

#### Testing
- Kernel is built successfully using project0 descriptions by updated `syscall_64.tbl` and `syscall.h` files.
- A `kqueue_syscalls.h` file is made to add kernel wrapper functions.
- The `kernel_tests.c` file facilitates the execution of all functions and system calls within the main program. 
- Three producer and two consumer kthreads are created to create and execute tasks.
- One slight issue is that I do not see the executed tasks in the completed queue even though according to my debugging statements, tasks are enqueued to the completed queue.
