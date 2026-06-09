#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

//  A valid 9x9 Sudoku grid for testing.
 
int sudoku[9][9] = {
    {6, 2, 4, 5, 3, 9, 1, 8, 7},
    {5, 1, 9, 7, 2, 8, 6, 3, 4},
    {8, 3, 7, 6, 1, 4, 2, 9, 5},
    {1, 4, 3, 8, 6, 5, 7, 2, 9},
    {9, 5, 8, 2, 4, 7, 3, 6, 1},
    {7, 6, 2, 3, 9, 1, 4, 5, 8},
    {3, 7, 1, 9, 5, 6, 8, 4, 2},
    {4, 9, 6, 1, 8, 2, 5, 7, 3},
    {2, 8, 5, 4, 7, 3, 9, 1, 6}
};

// Array to store results of 27 worker threads (0 = invalid, 1 = valid) 
int valid[27] = {0};

// Structure for passing data to threads 
typedef struct {
    int row;
    int column;
    int thread_id; 
} parameters;

// Thread function to check a specific row 
void *check_row(void *param) {
    parameters *data = (parameters *)param;
    int row = data->row;
    int id = data->thread_id;
    int check[10] = {0}; // Array to count occurrences of digits 1-9 

    for (int i = 0; i < 9; i++) {
        int val = sudoku[row][i];
        if (val < 1 || val > 9 || check[val] == 1) {
            pthread_exit(NULL); // Duplicate or out of bounds -> exit, valid[id] remains 0 
        }
        check[val] = 1;
    }
    valid[id] = 1; // Mark as valid 
    pthread_exit(NULL);
}

// Thread function to check a specific column 
void *check_column(void *param) {
    parameters *data = (parameters *)param;
    int col = data->column;
    int id = data->thread_id;
    int check[10] = {0};

    for (int i = 0; i < 9; i++) {
        int val = sudoku[i][col];
        if (val < 1 || val > 9 || check[val] == 1) {
            pthread_exit(NULL);
        }
        check[val] = 1;
    }
    valid[id] = 1;
    pthread_exit(NULL);
}

// Thread function to check a specific 3x3 subgrid 
void *check_square(void *param) {
    parameters *data = (parameters *)param;
    int start_row = data->row;
    int start_col = data->column;
    int id = data->thread_id;
    int check[10] = {0};

    for (int i = start_row; i < start_row + 3; i++) {
        for (int j = start_col; j < start_col + 3; j++) {
            int val = sudoku[i][j];
            if (val < 1 || val > 9 || check[val] == 1) {
                pthread_exit(NULL);
            }
            check[val] = 1;
        }
    }
    valid[id] = 1;
    pthread_exit(NULL);
}

// The parent thread 
int main() {
    pthread_t threads[27];
    int thread_idx = 0;

    // 1. Create 9 threads to check 9 rows 
    for (int i = 0; i < 9; i++) {
        parameters *data = (parameters *)malloc(sizeof(parameters));
        data->row = i;
        data->column = 0;
        data->thread_id = thread_idx;
        pthread_create(&threads[thread_idx++], NULL, check_row, data);
    }

    // 2. Create 9 threads to check 9 columns 
    for (int i = 0; i < 9; i++) {
        parameters *data = (parameters *)malloc(sizeof(parameters));
        data->row = 0;
        data->column = i;
        data->thread_id = thread_idx;
        pthread_create(&threads[thread_idx++], NULL, check_column, data);
    }

    // 3. Create 9 threads to check 9 3x3 subgrids 
    for (int i = 0; i < 9; i += 3) {
        for (int j = 0; j < 9; j += 3) {
            parameters *data = (parameters *)malloc(sizeof(parameters));
            data->row = i;
            data->column = j;
            data->thread_id = thread_idx;
            pthread_create(&threads[thread_idx++], NULL, check_square, data);
        }
    }

    // 4. Parent thread waits for all 27 worker threads to complete 
    for (int i = 0; i < 27; i++) {
        pthread_join(threads[i], NULL);
    }

    // 5. Aggregate results from the valid array 
    int is_valid = 1;
    for (int i = 0; i < 27; i++) {
        if (valid[i] == 0) {
            is_valid = 0;
            printf("Error found by thread ID: %d\n", i);
        }
    }

    // 6. Final verdict 
    if (is_valid) {
        printf("RESULT: The Sudoku solution is VALID!\n");
    } else {
        printf("RESULT: The Sudoku solution is INVALID!\n");
    }

    return 0;
}