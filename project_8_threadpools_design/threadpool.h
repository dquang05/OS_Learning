#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <semaphore.h>

// Number of threads in the pool 
#define NUMBER_OF_THREADS 3

// Size of the queue 
#define QUEUE_SIZE 15

// Represents a task to be executed 
typedef struct {
    void (*function)(void *p);
    void *data;
} task_struct;

// API functions 
int pool_init(void);
int pool_submit(void (*somefunction)(void *p), void *p);
void pool_shutdown(void);

#endif // THREADPOOL_H 