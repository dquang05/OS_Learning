#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// Function Prototypes 
int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);
int is_safe();
void print_state();

int main(int argc, char *argv[]) {
    // 1. Check command line arguments for initial available resources 
    if (argc != NUMBER_OF_RESOURCES + 1) {
        printf("Usage: %s <res1> <res2> <res3> <res4>\n", argv[0]);
        return -1;
    }

    // Initialize available array based on command line arguments 
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] = atoi(argv[i + 1]);
    }

    // 2. Read maximum array from file 
    FILE *file = fopen("max.txt", "r");
    if (!file) {
        printf("Error: Cannot open file 'max.txt'\n");
        return -1;
    }

    // Initialize maximum, allocation, and need arrays 
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            if (j == NUMBER_OF_RESOURCES - 1) {
                fscanf(file, "%d", &maximum[i][j]);
            } else {
                fscanf(file, "%d,", &maximum[i][j]);
            }
            allocation[i][j] = 0;
            need[i][j] = maximum[i][j];
        }
    }
    fclose(file);

    // 3. Interactive command loop 
    char command[10];
    int customer_num;
    int req_res[NUMBER_OF_RESOURCES];

    printf("Banker's Algorithm Simulator Started.\n");
    printf("Commands: RQ <cust> <r1> <r2> <r3> <r4> | RL <cust> <r1> <r2> <r3> <r4> | *\n");

    while (1) {
        printf("> ");
        scanf("%s", command);

        if (strcmp(command, "*") == 0) {
            print_state();
        } 
        else if (strcmp(command, "RQ") == 0) {
            scanf("%d %d %d %d %d", &customer_num, &req_res[0], &req_res[1], &req_res[2], &req_res[3]);
            if (request_resources(customer_num, req_res) == 0) {
                printf("Request granted. System is in a safe state.\n");
            } else {
                printf("Request denied. System would be in an unsafe state or invalid request.\n");
            }
        } 
        else if (strcmp(command, "RL") == 0) {
            scanf("%d %d %d %d %d", &customer_num, &req_res[0], &req_res[1], &req_res[2], &req_res[3]);
            release_resources(customer_num, req_res);
            printf("Resources released successfully.\n");
        } 
        else if (strcmp(command, "exit") == 0) {
            break;
        } 
        else {
            printf("Invalid command.\n");
        }
    }

    return 0;
}

// Core function to process resource requests 
int request_resources(int customer_num, int request[]) {
    // Step 1: Check if request > need 
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > need[customer_num][i]) {
            return -1; // Error: Process has exceeded its maximum claim 
        }
    }

    // Step 2: Check if request > available 
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > available[i]) {
            return -1; // Error: Resources are not available 
        }
    }

    // Step 3: Pretend to allocate resources 
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }

    // Step 4: Check if the new state is safe 
    if (is_safe() == 1) {
        return 0; // Safe: leave the allocation as is 
    } else {
        // Unsafe: Rollback the allocation 
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            available[i] += request[i];
            allocation[customer_num][i] -= request[i];
            need[customer_num][i] += request[i];
        }
        return -1;
    }
}

// Function to release resources back to the bank 
void release_resources(int customer_num, int release[]) {
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (release[i] <= allocation[customer_num][i]) {
            allocation[customer_num][i] -= release[i];
            available[i] += release[i];
            need[customer_num][i] += release[i];
        }
    }
}

// Safety Algorithm implementation 
int is_safe() {
    int work[NUMBER_OF_RESOURCES];
    int finish[NUMBER_OF_CUSTOMERS];

    // Initialize work and finish 
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        work[i] = available[i];
    }
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        finish[i] = 0; // 0 means false 
    }

    int count = 0;
    while (count < NUMBER_OF_CUSTOMERS) {
        int found = 0;
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
            if (finish[i] == 0) {
                int can_finish = 1;
                // Check if Need_i <= Work 
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                    if (need[i][j] > work[j]) {
                        can_finish = 0;
                        break;
                    }
                }

                // If resources can be allocated 
                if (can_finish == 1) {
                    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = 1;
                    found = 1;
                    count++;
                }
            }
        }
        // If no process was able to finish in this pass, state is unsafe 
        if (found == 0) {
            return 0; // Unsafe state 
        }
    }
    return 1; // Safe state 
}

// Helper function to print the current state of matrices 
void print_state() {
    printf("\nAvailable:\n");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        printf("%d ", available[i]);
    }
    
    printf("\n\nMaximum:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", maximum[i][j]);
        }
        printf("\n");
    }

    printf("\nAllocation:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", allocation[i][j]);
        }
        printf("\n");
    }

    printf("\nNeed:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}