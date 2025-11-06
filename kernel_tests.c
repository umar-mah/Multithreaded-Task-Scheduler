#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "kqueue_syscalls.h"


void generate_unique_resources(int *resources, int num_resources, int max_resources) {
    int used[max_resources];
    memset(used, 0, sizeof(used));

    for (int j = 0; j < num_resources; j++) {
        int resource;
        do {
            resource = rand() % max_resources;
        } while (used[resource]);

        used[resource] = 1;
        resources[j] = resource;
    }
}


void produce_task(int task_id, int priority, int duration, int num_resources) {
    int resources[num_resources];

    generate_unique_resources(resources, num_resources, 5);

    long result = kqueue_add_task(task_id, priority, duration, num_resources, resources);
    
    if (result == 0)
        printf("PRODUCER: Added task %d (Priority %d, Duration %d sec, Resources: %d)\n",
               task_id, priority, duration, num_resources);
    else
        printf("PRODUCER: Failed to add task %d: %s\n", task_id, strerror(errno));
}


int main() {
    printf("Starting Kernel Task Scheduler Test...\n");

    long init_result = kqueue_init_scheduler();
    if (init_result == 0) {
        printf("SCHEDULER: Initialized successfully.\n");
    } else {
        printf("SCHEDULER: Failed to initialize: %s\n", strerror(errno));
        return 1;
    }

    int num_tasks = 3;


    produce_task(1, 0, 3, 2);
    sleep(1);
    produce_task(2, 1, 3, 2);
    sleep(1);  
    produce_task(3, 2, 3, 2);
    sleep(1);  

    printf("\n--- SCHEDULING TASKS ---\n");

    long exit_result = kqueue_exit_scheduler();
    printf("\nTo see debugging outputs: run sudo dmesg | tail -50 command\n");
    if (exit_result == 0) {
        printf("SCHEDULER: Exited successfully.\n");
    } else {
        printf("SCHEDULER: Failed to exit: %s\n", strerror(errno));
    }

    printf("\nKernel Task Scheduler Test Complete.\n");
    return 0;
}

