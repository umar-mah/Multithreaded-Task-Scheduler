#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern queue_t high_priority_queue;
extern queue_t medium_priority_queue;
extern queue_t low_priority_queue;
extern queue_t waiting_queue;
extern queue_t completed_queue;
extern pthread_t producer_threads[8];
extern pthread_t consumer_threads[3];

void test_scheduler() {
    printf("Running scheduler with 8 producers and 3 consumers\n");
    printf("PRODUCER: Adding 8 tasks with a priority of HIGH(0), Medium(1), and LOW(2)\n");
    init_scheduler(8,3);

    sleep(20);
    print_completed_queue(&completed_queue);

    cleanup_scheduler(8,3);
    
    printf("\n");
    printf("\n");
    printf("Running scheduler with 6 producers and 3 consumers\n");
    printf("PRODUCER: Adding 6 tasks with a priority of HIGH(0), Medium(1), and LOW(2)\n");
    init_scheduler(6,3);

    sleep(20);
    print_completed_queue(&completed_queue);

    cleanup_scheduler(6,3);
}

int main() {
    test_scheduler();
    return 0;
}

void print_completed_queue(queue_t *queue) {
    pthread_mutex_lock(&queue->lock);

    task_t *current = queue->front;
    printf("\nCOMPLETED TASKS:\n");
    while (current) {
        printf("Task ID: %d | Priority: %d | Duration: %d sec | Resources Used: ",
               current->task_id, current->priority, current->duration);
        
        for (int i = 0; i < current->num_resources; i++) {
            printf("%d ", current->resources[i]);
        }
        printf("\n");

        current = current->next;
    }

    pthread_mutex_unlock(&queue->lock);
}

