#include "queue.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>  // For sleep
#include <stdio.h>   // For printf
#include <stdbool.h>

pthread_t producer_threads[8];
pthread_t consumer_threads[3];
sem_t queue_sem;
sem_t waiting_sem;
pthread_mutex_t task_id_mutex = PTHREAD_MUTEX_INITIALIZER;
int taskID = 0;

void generate_unique_resources(int *resources, int num_resources, int max_resources) {
    bool used[max_resources];  
    for (int i = 0; i < max_resources; i++) {
        used[i] = false;
    }

    for (int j = 0; j < num_resources; j++) {
        int resource;
        do {
            resource = rand() % max_resources;
        } while (used[resource]);  

        used[resource] = true;
        resources[j] = resource;
    }
}

void *task_producer(void *arg) {
    task_t *new_task = (task_t *)malloc(sizeof(task_t));

    pthread_mutex_lock(&task_id_mutex);
    new_task->task_id = ++taskID;  // unique task IDs
    pthread_mutex_unlock(&task_id_mutex);

    new_task->priority = rand() % 3;
    new_task->duration = (rand() % 5) + 1;
    new_task->num_resources = (rand() % 4) + 1;
    new_task->resources = (int *)malloc(new_task->num_resources * sizeof(int));

    generate_unique_resources(new_task->resources, new_task->num_resources, 5);  // unique resources

    new_task->next = NULL;

    if (new_task->priority == 0) enqueue(&high_priority_queue, new_task);
    else if (new_task->priority == 1) enqueue(&medium_priority_queue, new_task);
    else enqueue(&low_priority_queue, new_task);

    printf("CONSUMER: Added task with ID: %d\n", new_task->task_id);
    sem_post(&queue_sem);
    sleep(1);

    return NULL;
}

void *task_consumer(void *arg) {
    while (1) {
        sem_wait(&queue_sem);  // Waiting for tasks to be added to the queue

        pthread_mutex_lock(&task_id_mutex);
        task_t *task = NULL;

        // Dequeueing tasks from high, medium, low priority queues
        if (!is_queue_empty(&high_priority_queue)) {
            task = dequeue(&high_priority_queue);
            /*
            printf("CONSUMER: Added task with ID: %d Resources: ", task->task_id);
            for (int i = 0; i < task->num_resources; i++) {
            	printf("%d ", task->resources[i]);
            }
            printf("\n");
            */
        } else if (!is_queue_empty(&medium_priority_queue)) {
            task = dequeue(&medium_priority_queue);
              /*          printf("CONSUMER: Added task with ID: %d Resources: ", task->task_id);
            for (int i = 0; i < task->num_resources; i++) {
            	printf("%d ", task->resources[i]);
            }
            printf("\n");
            */
        } else if (!is_queue_empty(&low_priority_queue)) {
            task = dequeue(&low_priority_queue);
            /*
            printf("CONSUMER: Added task with ID: %d Resources: ", task->task_id);
            for (int i = 0; i < task->num_resources; i++) {
            	printf("%d ", task->resources[i]);
            }
            printf("\n");
            */
        }
        pthread_mutex_unlock(&task_id_mutex);  // Unlocking after dequeuing

        if (task) {
             //printf("SCHEDULER: Recieving Task %d (Priority %d)\n",
                       //task->task_id, task->priority);
            if (acquire_resources(task)) {
                printf("SCHEDULER: Executing Task %d (Priority %d) for %d seconds\n",
                       task->task_id, task->priority, task->duration);
                sleep(task->duration); 
                release_resources(task); 

                pthread_mutex_lock(&task_id_mutex);
                enqueue(&completed_queue, task); 
                pthread_mutex_unlock(&task_id_mutex);
            } else {
                //printf("SCHEDULER: Task %d moved to waiting queue\n", task->task_id);
                pthread_mutex_lock(&task_id_mutex);
                enqueue(&waiting_queue, task);  // Adding task to waiting queue
                pthread_mutex_unlock(&task_id_mutex);
                sem_post(&waiting_sem);  // Notifying waiting queue handler
            }
        } else {
            sleep(1);  // Wait
        }
    }
    return NULL;
}




void *waiting_queue_handler(void *arg) {
    while (1) {
        sem_wait(&waiting_sem);  // Waiting for a task in the waiting queue

        pthread_mutex_lock(&task_id_mutex);
        if (!is_queue_empty(&waiting_queue)) {
            task_t *task = dequeue(&waiting_queue);
           // printf("Waiting queue handler processing task %d (Priority %d)\n",
                   //task->task_id, task->priority);
            pthread_mutex_unlock(&task_id_mutex);

            if (acquire_resources(task)) {
                printf("WAITING SCHEDULER: Executing Task %d (Priority %d) from waiting queue for %d seconds\n",
                       task->task_id, task->priority, task->duration);
                sleep(task->duration);
                release_resources(task);

                pthread_mutex_lock(&task_id_mutex);
                enqueue(&completed_queue, task);  
                pthread_mutex_unlock(&task_id_mutex);
            } else {
                //printf("Task %d still waiting for resources, moving back to queue\n", task->task_id);
                pthread_mutex_lock(&task_id_mutex);
                if (task->priority == 0) 
                    enqueue(&high_priority_queue, task);
                else if (task->priority == 1) 
                    enqueue(&medium_priority_queue, task);
                else 
                    enqueue(&low_priority_queue, task);
                pthread_mutex_unlock(&task_id_mutex);
                
                sleep(1);  
                sem_post(&queue_sem);
                sem_post(&waiting_sem);  // Notifying that the task is still pending
            }
        } else {
            pthread_mutex_unlock(&task_id_mutex);
            sleep(1);  // Wait before checking again
        }
    }
    return NULL;
}






pthread_t waiting_thread;

void init_scheduler(int producers, int consumers) {
    sem_init(&waiting_sem, 0, 0); 
    sem_init(&queue_sem, 0, 0);
    init_queue(&high_priority_queue);
    init_queue(&medium_priority_queue);
    init_queue(&low_priority_queue);
    init_queue(&waiting_queue);
    init_queue(&completed_queue);
    init_resources(5);
    for (int i = 0; i < producers; i++) {
        pthread_create(&producer_threads[i], NULL, task_producer, NULL);
    }
    sleep(7);
    printf("PRODUCER: Executing Task Schedule\n");
    for (int i = 0; i < consumers; i++) {
        pthread_create(&consumer_threads[i], NULL, task_consumer, NULL);
    }

    pthread_create(&waiting_thread, NULL, waiting_queue_handler, NULL);
}



void cleanup_scheduler(int producers, int consumers) {
    for (int i = 0; i < producers; i++) {
        pthread_cancel(producer_threads[i]);
        pthread_join(producer_threads[i], NULL);
    }

    for (int i = 0; i < consumers; i++) {
        pthread_cancel(consumer_threads[i]);
        pthread_join(consumer_threads[i], NULL);
    }

    pthread_cancel(waiting_thread);
    pthread_join(waiting_thread, NULL);

    task_t *task;
    while ((task = dequeue(&high_priority_queue)) || (task = dequeue(&medium_priority_queue)) ||
           (task = dequeue(&low_priority_queue)) || (task = dequeue(&waiting_queue)) ||
           (task = dequeue(&completed_queue))) {
        if (task->resources) free(task->resources);
        free(task);
    }

    resource_t *resource = resource_queue.resources;
    while (resource) {
        resource_t *temp = resource;
        resource = resource->next;
        sem_destroy(&temp->semaphore);
        free(temp);
    }
    sem_destroy(&queue_sem);
    sem_destroy(&waiting_sem); 
    printf("Scheduler Cleanup complete\n");
}


