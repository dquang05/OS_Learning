#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

// The thread pool array 
pthread_t workers[NUMBER_OF_THREADS];
void execute(void (*somefunction)(void *p), void *p);
// The task queue 
task_struct task_queue[QUEUE_SIZE];
int head = 0; // Points to where to dequeue 
int tail = 0; // Points to where to enqueue 
int count = 0; // Number of tasks in queue 

// Synchronization tools 
pthread_mutex_t queue_mutex;
sem_t queue_sem;

// Worker thread execution function 
 
void *worker(void *param) {
    task_struct task;

    while (1) {
        // Wait for a task to become available (semaphore) 
        sem_wait(&queue_sem);

        // Lock the queue before modifying it 
        pthread_mutex_lock(&queue_mutex);

        // Dequeue the task 
        task = task_queue[head];
        head = (head + 1) % QUEUE_SIZE;
        count--;

        // Unlock the queue 
        pthread_mutex_unlock(&queue_mutex);

        // Execute the task 
        execute(task.function, task.data);
    }
    pthread_exit(0);
}

// Helper function
 
void execute(void (*somefunction)(void *p), void *p) {
    (*somefunction)(p);
}

// Initializes the thread pool, mutex, and semaphore 
 
int pool_init(void) {
    // Initialize mutex and semaphore 
    if (pthread_mutex_init(&queue_mutex, NULL) != 0) {
        return 1;
    }
    // Initialize semaphore to 0 (no tasks initially) 
    if (sem_init(&queue_sem, 0, 0) != 0) {
        return 1;
    }

    // Create the worker threads 
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_create(&workers[i], NULL, worker, NULL);
    }

    return 0;
}



int pool_submit(void (*somefunction)(void *p), void *p) {
    
    pthread_mutex_lock(&queue_mutex);
    if(count == QUEUE_SIZE) {
        pthread_mutex_unlock(&queue_mutex);
        return 1; // Queue is full
    }
    task_struct new_task;
    new_task.function = somefunction;
    new_task.data = p;
    task_queue[tail] = new_task;
    tail = (tail + 1) % QUEUE_SIZE;
    count++;
    pthread_mutex_unlock(&queue_mutex);
    sem_post(&queue_sem); // Wake up one worker thread
    return 0; 
}


void pool_shutdown(void) {
    for(int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_cancel(workers[i]);
    }
    for(int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_join(workers[i], NULL);
    }
    pthread_mutex_destroy(&queue_mutex);
    sem_destroy(&queue_sem);
}