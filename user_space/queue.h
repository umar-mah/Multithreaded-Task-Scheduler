#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <semaphore.h>

// Task Structure
typedef struct task {
    int task_id;
    int priority;
    int duration;
    int *resources;
    int num_resources;
    struct task *next;
} task_t;

// Queue Structure
typedef struct queue {
    task_t *front;
    task_t *rear;
    pthread_mutex_t lock;
} queue_t;

// Resource Structure
typedef struct resource {
    int rid;
    sem_t semaphore;
    struct resource *next;
} resource_t;

typedef struct resource_queue {
    resource_t *resources;
    pthread_mutex_t lock;
} resource_queue_t;

// Declare external queue variables
extern queue_t high_priority_queue;
extern queue_t medium_priority_queue;
extern queue_t low_priority_queue;
extern queue_t waiting_queue;
extern queue_t completed_queue;

extern resource_queue_t resource_queue;

// Function Declarations
void init_queue(queue_t *queue);
void enqueue(queue_t *queue, task_t *task);
task_t *dequeue(queue_t *queue);
int is_queue_empty(queue_t *queue);

void init_resources(int num_resources);
resource_t *get_resource(int rid);
int acquire_resources(task_t *task);
void release_resources(task_t *task);

#endif

