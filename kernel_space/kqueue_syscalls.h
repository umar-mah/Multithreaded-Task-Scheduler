#ifndef KQUEUE_WRAPPER_H
#define KQUEUE_WRAPPER_H

#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

#define SYS_add_task 449          
#define SYS_schedule_task 450
#define SYS_acquire_resources 451
#define SYS_release_resources 452
#define SYS_init_scheduler 453    
#define SYS_exit_scheduler 454    

static inline long kqueue_add_task(int task_id, int priority, int duration, int num_resources, int *resources) {
    return syscall(SYS_add_task, task_id, priority, duration, num_resources, resources);
}

static inline long kqueue_schedule_task(void) {
    return syscall(SYS_schedule_task);
}

static inline long kqueue_acquire_resources(int task_id) {
    return syscall(SYS_acquire_resources, task_id);
}

static inline long kqueue_release_resources(int task_id) {
    return syscall(SYS_release_resources, task_id);
}

static inline long kqueue_init_scheduler(void) {
    return syscall(SYS_init_scheduler);  
}

static inline long kqueue_exit_scheduler(void) {
    return syscall(SYS_exit_scheduler);  
}

#endif 

