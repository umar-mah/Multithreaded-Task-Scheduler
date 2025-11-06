#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

// Define the global queues
queue_t high_priority_queue;
queue_t medium_priority_queue;
queue_t low_priority_queue;
queue_t waiting_queue;
queue_t completed_queue;

// Define the global resource queue
resource_queue_t resource_queue;

void init_queue(queue_t *queue) {
    queue->front = NULL;
    queue->rear = NULL;
    pthread_mutex_init(&queue->lock, NULL);
}

void enqueue(queue_t *queue, task_t *task) {
    pthread_mutex_lock(&queue->lock);
    if (queue->rear == NULL) {
        queue->front = task;
        queue->rear = task;
    } else {
        queue->rear->next = task;
        queue->rear = task;
    }
    task->next = NULL;
    pthread_mutex_unlock(&queue->lock);
}

task_t *dequeue(queue_t *queue) {
    pthread_mutex_lock(&queue->lock);
    if (queue->front == NULL) {
        pthread_mutex_unlock(&queue->lock);
        return NULL;
    }
    task_t *task = queue->front;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    pthread_mutex_unlock(&queue->lock);
    return task;
}

int is_queue_empty(queue_t *queue) {
    pthread_mutex_lock(&queue->lock);
    int empty = (queue->front == NULL);
    pthread_mutex_unlock(&queue->lock);
    return empty;
}

void init_resources(int num_resources) {
    resource_queue.resources = NULL;
    pthread_mutex_init(&resource_queue.lock, NULL);

    for (int i = 0; i < num_resources; i++) { 
        resource_t *new_resource = (resource_t *)malloc(sizeof(resource_t));
        new_resource->rid = i;
        sem_init(&new_resource->semaphore, 0, 1);
        new_resource->next = resource_queue.resources;
        resource_queue.resources = new_resource;
    }
}

resource_t *get_resource(int rid) {
    resource_t *current = resource_queue.resources;
    while (current) {
        if (current->rid == rid) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int acquire_resources(task_t *task) {
    for (int i = 0; i < task->num_resources; i++) {
        resource_t *res = get_resource(task->resources[i]);
        if (!res || sem_trywait(&res->semaphore) != 0) {
            for (int j = 0; j < i; j++) {
                sem_post(&get_resource(task->resources[j])->semaphore);
            }
            return 0;
        }
    }
    return 1;
}

void release_resources(task_t *task) {
    for (int i = 0; i < task->num_resources; i++) {
        sem_post(&get_resource(task->resources[i])->semaphore);
    }
}

