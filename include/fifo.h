#pragma once

#include <semaphore.h>

// FIFO queue
typedef struct {
    int* buf;  // Buffer that holds the data
    int head;  // Index of the first element
    int tail;  // Index of the next free element
    int size;  // Size of the buffer
} fifo_t;

// Initialize the queue (allocates memory)
void fifo_init(fifo_t* f, int size);

// Destroy the queue (frees memory)
void fifo_destroy(fifo_t* fifo);

// Push data into the queue (does not check for overflow)
void fifo_push(fifo_t* f, int data);

// Pop data from the queue (does not check for underflow)
int fifo_pop(fifo_t* f);

// Return number of elements in the queue
int fifo_count(fifo_t* f);

// Return the first element in the queue (does not check if empty)
int fifo_top(fifo_t* f);

// Count odd numbers in a FIFO queue
int fifo_count_odd(fifo_t* f);

// Count even numbers in a FIFO queue
int fifo_count_even(fifo_t* f);