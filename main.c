#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#define MATRIX_SIZE 100
#define VALUE_SIZE 40
#define NUM_PROCESSES 4
#define NUM_THREADS 4

// Function pointer type for functions that take no arguments and return void
typedef void (*FunctionPointer)();

// Matrices for input, output, and results of different multiplication approaches
int matrixA[MATRIX_SIZE][MATRIX_SIZE];
int matrixB[MATRIX_SIZE][MATRIX_SIZE];
int resultByNaiveApproach[MATRIX_SIZE][MATRIX_SIZE];
int resultByProcesses[MATRIX_SIZE][MATRIX_SIZE];
int resultByJoinThreads[MATRIX_SIZE][MATRIX_SIZE];
int resultByMixThreads[MATRIX_SIZE][MATRIX_SIZE];
int resultByDetachedThreads[MATRIX_SIZE][MATRIX_SIZE];

// Function to initialize a matrix with random values based on a string
void initializeMatrix(int matrix[MATRIX_SIZE][MATRIX_SIZE], char string[VALUE_SIZE]);

// Function to check if two matrices are equal
void areResultsEqual(int M1[MATRIX_SIZE][MATRIX_SIZE], int M2[MATRIX_SIZE][MATRIX_SIZE]);

// Function to print a matrix
void printMatrix(int matrix[MATRIX_SIZE][MATRIX_SIZE]);

// Function to measure the execution time of a given function
double measureExecutionTime(FunctionPointer func);

// Function to perform matrix multiplication using the naive approach
void multiplyMatrices();

// Function to perform matrix multiplication using processes
void parallelMMWithProcesses();

// Function to perform matrix multiplication using threads with join
void parallelMMWithJoinThreads();
void* joinThreadMM(void* arg);

// Function to perform matrix multiplication using mixed threads (combination of join and detached)
void parallelMMWithMixThreads();
void* mixThreadMM(void* arg);

// Function to perform matrix multiplication using detached threads
void parallelMMWithDetachedThreads();
void* detachedThreadMM(void* arg);

int main() {
    // Input numbers as strings
    char myNumber[] = "1211553";
    char birthYear[] = "2003";

    // Convert strings to long long integers
    long long num1 = atoi(myNumber);
    long long num2 = atoi(birthYear);
    long long result = num1 * num2;

    // Convert the result back to a string
    char resultStr[VALUE_SIZE];
    sprintf(resultStr, "%lld", result);

    // Initialize matrices with the input numbers and the result
    initializeMatrix(matrixA, myNumber);
    initializeMatrix(matrixB, resultStr);

    printf("QOSSAY RIDI %s\n", myNumber);

    // Measure and display the execution time of the naive matrix multiplication
    double naiveTime = measureExecutionTime(multiplyMatrices);
    printf("Naive solution:\n\tNaive approach: %f seconds\n\n", naiveTime);

    // Measure and display the execution time of matrix multiplication with processes
    double processTime = measureExecutionTime(parallelMMWithProcesses);
    printf("Processes solution:\n\tProcess-based approach with %d processes: %f seconds\n", NUM_PROCESSES, processTime);

    // Check if results from naive and process approaches are equal
    areResultsEqual(resultByNaiveApproach, resultByProcesses);

    // Measure and display the execution time of matrix multiplication with join threads
    double joinThreadTime = measureExecutionTime(parallelMMWithJoinThreads);
    printf("Threads solution:\n\t1- Thread-based approach with %d join threads: %f seconds\n", NUM_THREADS, joinThreadTime);

    // Check if results from naive and thread (join) approaches are equal
    areResultsEqual(resultByNaiveApproach, resultByJoinThreads);

    // Perform matrix multiplication with mixed (join and detached) threads
    parallelMMWithMixThreads();

    // Measure and display the execution time of matrix multiplication with detached threads
    double detachedThreadTime = measureExecutionTime(parallelMMWithDetachedThreads);
    printf("\n\t3- Thread-based approach with %d detached threads: such as time of creation = %f seconds\n", NUM_THREADS, detachedThreadTime);

    // Exit the main thread ; this allow to wait all thread not finished
    pthread_exit(0);
}
// Function to initialize a matrix with repeating digits from a string
void initializeMatrix(int matrix[MATRIX_SIZE][MATRIX_SIZE], char string[VALUE_SIZE]) {
    int size = 0;

    // Calculate the size of the string
    while (string[size] != '\0')
        size++;

    int index = 0;

    // Fill the matrix with repeating digits from the string
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix[i][j] = string[index] - '0';
            index = (index + 1) % size;
        }
    }
}

// Function to measure the execution time of a given function
double measureExecutionTime(FunctionPointer func) {
    struct timespec start_time, end_time;

    // Record the start time
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Execute the specified function
    func();

    // Record the end time
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // Calculate and return the elapsed time in seconds
    return (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
}

// Function to check if two matrices are equal
void areResultsEqual(int M1[MATRIX_SIZE][MATRIX_SIZE], int M2[MATRIX_SIZE][MATRIX_SIZE]) {
    int flag = 1;

    // Compare each element of the matrices
    for (int i = 0; i < MATRIX_SIZE; i++)
        for (int j = 0; j < MATRIX_SIZE; j++)
            if (M1[i][j] != M2[i][j])
                flag = 0;

    // Print the result of the equality check
    if (flag == 1)
        printf("\t\tThe result is identical to that of the naive approach\n\n");
    else
        printf("\t\tThe result isn't identical to that of the naive approach\n\n");
}

// Function to print a matrix
void printMatrix(int matrix[MATRIX_SIZE][MATRIX_SIZE]) {
    // Print each element of the matrix
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

// ******************************************************************************************************
// ******************************************************************************************************
//              A naive approach, matrix multiplication in a conventional way
// ******************************************************************************************************
// ******************************************************************************************************

void multiplyMatrices() {
    // Iterate through each element of the result matrix
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            resultByNaiveApproach[i][j] = 0;
            // Perform the multiplication and accumulate the result
            for (int k = 0; k < MATRIX_SIZE; k++)
                resultByNaiveApproach[i][j] += matrixA[i][k] * matrixB[k][j];

        }
    }
}

//******************************************************************************************************
//******************************************************************************************************
//                  Processes approach, using several processes to multiply matrices
//******************************************************************************************************
//******************************************************************************************************

void parallelMMWithProcesses() {
    pid_t child_pid = 0;
    int status;
    int pipes[NUM_PROCESSES][2];

    // Create pipes for communication between parent and child processes
    for (int i = 0; i < NUM_PROCESSES; i++) {
        if (pipe(pipes[i]) == -1) {
            printf("Pipe failed");
            return;
        }

        // Fork a child process
        child_pid = fork();

        if (child_pid == 0) {
            // Code executed by child processes

            // Calculate the range of rows to be processed by each child
            int start_row = i * (MATRIX_SIZE / NUM_PROCESSES);
            int end_row = (i + 1) * (MATRIX_SIZE / NUM_PROCESSES);

            // Adjust the end_row for the last process
            if (i == NUM_PROCESSES - 1)
                end_row = MATRIX_SIZE;

            // Perform matrix multiplication for the assigned rows
            for (int j = start_row; j < end_row; j++) {
                for (int k = 0; k < MATRIX_SIZE; k++) {
                    resultByProcesses[j][k] = 0;
                    for (int l = 0; l < MATRIX_SIZE; l++)
                        resultByProcesses[j][k] += matrixA[j][l] * matrixB[l][k];
                }
            }

            // Write the result to the pipe and exit the child process
            write(pipes[i][1], resultByProcesses + start_row, (end_row - start_row) * MATRIX_SIZE * sizeof(int));
            close(pipes[i][0]); // Close the reading end of the pipe
            close(pipes[i][1]); // Close the writing end of the pipe
            exit(0);
        }
    }

    // Code executed by the parent proess

    // Wait for all child processes to finish
    for (int i = 0; i < NUM_PROCESSES; i++) {
        wait(&status);

        // Calculate the range of rows to be read from each child
        int start_row = i * (MATRIX_SIZE / NUM_PROCESSES);
        int end_row = (i + 1) * (MATRIX_SIZE / NUM_PROCESSES);

        // Adjust the end_row for the last process
        if (i == NUM_PROCESSES - 1)
            end_row = MATRIX_SIZE;

        // Read the result from the pipe and close the pipe
        read(pipes[i][0], resultByProcesses + start_row, (end_row - start_row) * MATRIX_SIZE * sizeof(int));
        close(pipes[i][0]); // Close the reading end of the pipe
        close(pipes[i][1]); // Close the writing end of the pipe
    }
}

//******************************************************************************************************
//******************************************************************************************************
//                 Join threads approach, using several join threads to multiply matrices
//******************************************************************************************************
//******************************************************************************************************

void parallelMMWithJoinThreads() {
    // Array to store thread identifiers and an array for thread IDs
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, joinThreadMM, &thread_ids[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}

void* joinThreadMM(void* arg) {
    // Extract the thread ID from the argument
    int thread_id = *((int*)arg);

    // Calculate the range of rows to be processed by this thread
    int start_row = thread_id * (MATRIX_SIZE / NUM_THREADS);
    int end_row = (thread_id + 1) * (MATRIX_SIZE / NUM_THREADS);

    // Adjust the end_row for the last thread
    if (thread_id == NUM_THREADS - 1)
        end_row = MATRIX_SIZE;

    // Perform matrix multiplication for the assigned rows
    for (int j = start_row; j < end_row; j++) {
        for (int k = 0; k < MATRIX_SIZE; k++) {
            resultByJoinThreads[j][k] = 0;
            for (int l = 0; l < MATRIX_SIZE; l++)
                resultByJoinThreads[j][k] += matrixA[j][l] * matrixB[l][k];
        }
    }

    return NULL;
}

//******************************************************************************************************
//******************************************************************************************************
//       Mix of join & detached threads approach, using several threads to multiply matrices
//******************************************************************************************************
//******************************************************************************************************

void parallelMMWithMixThreads() {
    // Print information about the mixed approach
    printf("\t2- Thread-based approach with mix join & detached threads\n");

    // Array to store thread identifiers for detached threads
    pthread_t threads[NUM_THREADS];

    // Allocate memory for thread IDs for detached threads
    int* detachedThread_ids = (int*)malloc(NUM_THREADS / 2 * sizeof(int));

    // Create and detach detached threads
    for (int i = 0; i < NUM_THREADS / 2; i++) {
        detachedThread_ids[i] = i;
        pthread_create(&threads[i], NULL, mixThreadMM, &detachedThread_ids[i]);
        pthread_detach(threads[i]);
    }

    // Array to store thread IDs for join threads
    int joinThread_ids[NUM_THREADS / 2];

    // Create join threads
    for (int i = NUM_THREADS / 2; i < NUM_THREADS; i++) {
        joinThread_ids[i - NUM_THREADS / 2] = i;
        pthread_create(&threads[i], NULL, mixThreadMM, &joinThread_ids[i - NUM_THREADS / 2]);
    }

    // Wait for join threads to finish
    for (int i = NUM_THREADS / 2; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}

void* mixThreadMM(void* arg) {
    // Extract the thread ID from the argument
    int thread_id = *((int*)arg);

    // Record the start time
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Calculate the range of rows to be processed by this thread
    int start_row = thread_id * (MATRIX_SIZE / NUM_THREADS);
    int end_row = (thread_id + 1) * (MATRIX_SIZE / NUM_THREADS);

    // Adjust the end_row for the last thread
    if (thread_id == NUM_THREADS - 1)
        end_row = MATRIX_SIZE;

    // Perform matrix multiplication for the assigned rows
    for (int j = start_row; j < end_row; j++) {
        for (int k = 0; k < MATRIX_SIZE; k++) {
            resultByMixThreads[j][k] = 0;
            for (int l = 0; l < MATRIX_SIZE; l++)
                resultByMixThreads[j][k] += matrixA[j][l] * matrixB[l][k];
        }
    }

    // Record the end time
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // Calculate the elapsed time in seconds
    double time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    // Check if the result matches the naive approach
    int flag = 1;
    for (int i = start_row; i < end_row; i++)
        for (int j = 0; j < MATRIX_SIZE; j++)
            if (resultByMixThreads[i][j] != resultByNaiveApproach[i][j])
                flag = 0;

    // Print information about the thread's work completion
    if (flag && thread_id < NUM_THREADS / 2) {
        printf("\t\tFrom detached thread No. %d, ", thread_id);
        printf("The required work was completed successfully within %f seconds\n", time);
    } else if (flag) {
        printf("\t\tFrom join thread No. %d, ", thread_id);
        printf("The required work was completed successfully within %f seconds\n", time);
    } else {
        printf("\t\tFrom thread No. %d, There is an error that has occurred\n", thread_id);
    }

    return NULL;
}

//******************************************************************************************************
//******************************************************************************************************
//              Detached threads approach, using several detached threads to multiply matrices
//******************************************************************************************************
//******************************************************************************************************

void parallelMMWithDetachedThreads() {
    // Array to store thread identifiers and allocate memory for thread IDs
    pthread_t threads[NUM_THREADS];
    int* thread_ids = (int*)malloc(NUM_THREADS * sizeof(int));

    // Create and detach detached threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, detachedThreadMM, &thread_ids[i]);
        pthread_detach(threads[i]);
    }
}

void* detachedThreadMM(void* arg) {
    // Extract the thread ID from the argument
    int thread_id = *((int*)arg);

    // Record the start time
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Calculate the range of rows to be processed by this thread
    int start_row = thread_id * (MATRIX_SIZE / NUM_THREADS);
    int end_row = (thread_id + 1) * (MATRIX_SIZE / NUM_THREADS);

    // Adjust the end_row for the last thread
    if (thread_id == NUM_THREADS - 1)
        end_row = MATRIX_SIZE;

    // Perform matrix multiplication for the assigned rows
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            resultByDetachedThreads[i][j] = 0;
            for (int k = 0; k < MATRIX_SIZE; k++)
                resultByDetachedThreads[i][j] += matrixA[i][k] * matrixB[k][j];
        }
    }

    // Record the end time
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // Calculate the elapsed time in seconds
    double time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    // Check if the result matches the naive approach
    int flag = 1;
    for (int i = start_row; i < end_row; i++)
        for (int j = 0; j < MATRIX_SIZE; j++)
            if (resultByDetachedThreads[i][j] != resultByNaiveApproach[i][j])
                flag = 0;

    // Print information about the thread's work completion
    if (flag) {
        printf("\t\tFrom detached thread No. %d, ", thread_id);
        printf("The required work was completed successfully within %f seconds\n", time);
    } else {
        printf("\t\tFrom detached thread No. %d, There is an error that has occurred\n", thread_id);
    }

    return NULL;
}
