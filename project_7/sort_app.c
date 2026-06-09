#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 10

// Global arrays shared across all threads 
int list[SIZE] = {7, 12, 19, 3, 18, 4, 2, 6, 15, 8};
int result[SIZE];

// Structure for passing parameters to threads 
typedef struct {
    int start;
    int end;
} parameters;

// Thread function to sort a sublist 
void *sorter(void *params) {
    parameters *p = (parameters *)params;
    int begin = p->start;
    int end = p->end;
    
    // Simple Bubble Sort for the assigned half of the array 
    for (int i = begin; i <= end; i++) {
        for (int j = begin; j < end - (i - begin); j++) {
            if (list[j] > list[j + 1]) {
                // Swap elements 
                int temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
    pthread_exit(0);
}

// Thread function to merge two sorted sublists 
void *merger(void *params) {
    // Define boundaries for the two halves 
    int begin1 = 0;
    int end1 = (SIZE / 2) - 1;
    int begin2 = SIZE / 2;
    int end2 = SIZE - 1;
    
    int i = begin1;
    int j = begin2;
    int k = 0;
    
    // Merge the two halves into the result array 
    while (i <= end1 && j <= end2) {
        if (list[i] < list[j]) {
            result[k++] = list[i++];
        } else {
            result[k++] = list[j++];
        }
    }
    
    // Copy remaining elements from the first half, if any 
    while (i <= end1) {
        result[k++] = list[i++];
    }
    
    // Copy remaining elements from the second half, if any 
    while (j <= end2) {
        result[k++] = list[j++];
    }
    
    pthread_exit(0);
}

int main() {
    pthread_t tid1, tid2, tid3;
    
    // Print original array 
    printf("Original array: ");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", list[i]);
    }
    printf("\n");

    // 1. Allocate memory and set parameters for Sorting Thread 1 
    parameters *data1 = (parameters *)malloc(sizeof(parameters));
    data1->start = 0;
    data1->end = (SIZE / 2) - 1;

    // 2. Allocate memory and set parameters for Sorting Thread 2 
    parameters *data2 = (parameters *)malloc(sizeof(parameters));
    data2->start = SIZE / 2;
    data2->end = SIZE - 1;

    // 3. Create sorting threads 
    pthread_create(&tid1, NULL, sorter, data1);
    pthread_create(&tid2, NULL, sorter, data2);

    // 4. Wait for both sorting threads to finish (Synchronization Barrier) 
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    // 5. Create merging thread 
    // No parameters needed as boundaries are calculated inside the function 
    pthread_create(&tid3, NULL, merger, NULL);

    // 6. Wait for merging thread to finish 
    pthread_join(tid3, NULL);

    // 7. Output the sorted result 
    printf("Sorted array:   ");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");

    // Clean up dynamically allocated memory 
    free(data1);
    free(data2);

    return 0;
}