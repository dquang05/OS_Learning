/*This is client code to simulate a simple thread pool usage*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "threadpool.h"

// Define
typedef struct {
    int id;
    int a;
    int b;
} task_data;

// Add task
void my_add_task(void *param) {
   
    task_data *data = (task_data *)param;
    
    // Print log
    printf("[Worker %lu] processing ID %d: %d + %d = %d\n", 
           pthread_self(), data->id, data->a, data->b, data->a + data->b);
    
    // Simulate time-consuming task by sleeping for 1-2 seconds
    sleep(rand() % 2 + 1); 
    
    free(data);
}

int main(void) {
    printf("Init threadpool\n");
    srand(time(NULL));
    
    if (pool_init() != 0) {
        printf("Error initializing thread pool\n");
        return 1;
    }
    printf("Initialized, %d worker threads\n\n", NUMBER_OF_THREADS);

    int num_tasks = 20; 

    for (int i = 0; i < num_tasks; i++) {
        task_data *data = (task_data *)malloc(sizeof(task_data));
        data->id = i + 1;
        data->a = rand() % 100; 
        data->b = rand() % 100;

        if (pool_submit(my_add_task, data) != 0) {
            printf("Queue is full, rejecting Task ID %d\n", data->id);
            free(data); 
        } else {
            printf("[Client] -> Submitted Task ID %d to the queue.\n", data->id);
        }
        
        // delay a bit before submitting the next task to simulate real-time task submission
        usleep(200000); 
    }

    printf("\nwaiting for tasks to complete...\n");
    sleep(5); // Wait for a while to let worker threads finish processing tasks

    printf("\nAll tasks completed.\n");
    pool_shutdown();
    printf("Pool shutdown complete.\n");

    return 0;
}