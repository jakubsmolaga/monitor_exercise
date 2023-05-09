#include <fifo.h>
#include <monitor.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <unistd.h>

/* -------------------------------- Variables ------------------------------- */

fifo_t fifo;        // FIFO queue
monitor_t monitor;  // Monitor

cond_t cond_prod_even;  // Condition for producers to produce even numbers
cond_t cond_cons_even;  // Condition for consumers to consume even numbers
cond_t cond_prod_odd;   // Condition for producers to produce odd numbers
cond_t cond_cons_odd;   // Condition for consumers to consume odd numbers

/* --------------------------------- Helpers -------------------------------- */

// Sleep for a random amount of time (10-20 ms)
void rand_sleep() {
    usleep(10000 + rand() % 10000);
};

// Check if the FIFO queue is full
int fifo_full() {
    return fifo_count(&fifo) == fifo.size;
};

/* ------------------------------- Predicates ------------------------------- */

// Returns 1 if a producer can produce an even number, 0 otherwise
int can_prod_even() {
    return !fifo_full() && fifo_count_even(&fifo) < 10;
};

// Returns 1 if a producer can produce an odd number, 0 otherwise
int can_prod_odd() {
    return !fifo_full() && fifo_count_odd(&fifo) < fifo_count_even(&fifo);
};

// Returns 1 if a consumer can consume an even number, 0 otherwise
int can_cons_even() {
    return fifo_count(&fifo) > 3 && fifo_top(&fifo) % 2 == 0;
};

// Returns 1 if a consumer can consume an odd number, 0 otherwise
int can_cons_odd() {
    return fifo_count(&fifo) > 7 && fifo_top(&fifo) % 2 == 1;
};

/* ----------------------------- Monitor Update ----------------------------- */

// Updates the monitor
// This function should be called after every change to the FIFO queue
// Requires the monitor to be locked
void monitor_update() {
    // Signal the first condition that can be signaled
    if (can_prod_even() && monitor_signal(&monitor, &cond_prod_even))
        return;
    if (can_prod_odd() && monitor_signal(&monitor, &cond_prod_odd))
        return;
    if (can_cons_even() && monitor_signal(&monitor, &cond_cons_even))
        return;
    if (can_cons_odd() && monitor_signal(&monitor, &cond_cons_odd))
        return;
    // If no condition can be signaled, unlock the monitor
    monitor_leave(&monitor);
};

/* ------------------------------- Producers ------------------------------- */

// Producer thread that produces even numbers
void* prod_even(void* ctx) {
    int i = 0;
    while (1) {
        monitor_enter(&monitor);
        if (!can_prod_even())
            monitor_wait(&monitor, &cond_prod_even);
        fifo_push(&fifo, i);
        printf("prod_even: %d\n", i);
        i += 2;
        i %= 50;
        monitor_update();
        rand_sleep();
    }
};

// Producer thread that produces odd numbers
void* prod_odd(void* ctx) {
    int i = 1;
    while (1) {
        monitor_enter(&monitor);
        if (!can_prod_odd())
            monitor_wait(&monitor, &cond_prod_odd);
        fifo_push(&fifo, i);
        printf("prod_odd: %d\n", i);
        i += 2;
        i %= 50;
        monitor_update();
        rand_sleep();
    }
};

/* ------------------------------- Consumers ------------------------------- */

// Consumer thread that consumes even numbers
void* cons_even(void* ctx) {
    while (1) {
        monitor_enter(&monitor);
        if (!can_cons_even())
            monitor_wait(&monitor, &cond_cons_even);
        int data = fifo_pop(&fifo);
        printf("cons_even: %d\n", data);
        monitor_update();
        rand_sleep();
    }
};

// Consumer thread that consumes odd numbers
void* cons_odd(void* ctx) {
    while (1) {
        monitor_enter(&monitor);
        if (!can_cons_odd())
            monitor_wait(&monitor, &cond_cons_odd);
        int data = fifo_pop(&fifo);
        printf("cons_odd: %d\n", data);
        monitor_update();
        rand_sleep();
    }
};

/* ---------------------------------- Setup --------------------------------- */

// Initialize all variables before running a test
void test_init(int size) {
    fifo_init(&fifo, size);      // Initialize the FIFO queue
    monitor_init(&monitor);      // Initialize the monitor
    cond_init(&cond_prod_even);  // Initialize the prod_even condition
    cond_init(&cond_cons_even);  // Initialize the cons_even condition
    cond_init(&cond_prod_odd);   // Initialize the prod_odd condition
    cond_init(&cond_cons_odd);   // Initialize the cons_odd condition
};

// Cleanup all variables after running a test
void test_destroy() {
    fifo_destroy(&fifo);            // Destroy the FIFO queue
    monitor_destroy(&monitor);      // Destroy the monitor
    cond_destroy(&cond_prod_even);  // Destroy the prod_even condition
    cond_destroy(&cond_cons_even);  // Destroy the cons_even condition
    cond_destroy(&cond_prod_odd);   // Destroy the prod_odd condition
    cond_destroy(&cond_cons_odd);   // Destroy the cons_odd condition
};

/* ---------------------------------- Tests --------------------------------- */

// TEST1
//  Only prod_even is running
//  Should produce 10 even numbers
void test_1() {
    printf("Test 1\n");
    test_init(11);
    pthread_t thread_prod_even;
    pthread_create(&thread_prod_even, NULL, prod_even, NULL);
    sleep(1);
    if (fifo_count(&fifo) != 10 || fifo_count_even(&fifo) != 10) {
        printf("FAILED\n");
    } else {
        printf("OK\n");
    }
    pthread_cancel(thread_prod_even);
    test_destroy();
};

// TEST2
// Only prod_odd is running
// Shouldn't produce any numbers
void test_2() {
    printf("Test 2\n");
    test_init(1);
    pthread_t thread_prod_odd;
    pthread_create(&thread_prod_odd, NULL, prod_odd, NULL);
    sleep(1);
    if (fifo_count(&fifo) != 0) {
        printf("FAILED\n");
    } else {
        printf("OK\n");
    }
    pthread_cancel(thread_prod_odd);
    test_destroy();
};

// TEST3
// Only cons_even is running
// Shouldn't consume any numbers
void test_3() {
    printf("Test 3\n");
    test_init(1);
    pthread_t thread_cons_even;
    pthread_create(&thread_cons_even, NULL, cons_even, NULL);
    sleep(1);
    if (fifo_count(&fifo) != 0) {
        printf("FAILED\n");
    } else {
        printf("OK\n");
    }
    pthread_cancel(thread_cons_even);
    test_destroy();
};

// TEST4
// Only cons_odd is running
// Shouldn't consume any numbers
void test_4() {
    printf("Test 4\n");
    test_init(1);
    pthread_t thread_cons_odd;
    pthread_create(&thread_cons_odd, NULL, cons_odd, NULL);
    sleep(1);
    if (fifo_count(&fifo) != 0) {
        printf("FAILED\n");
    } else {
        printf("OK\n");
    }
    pthread_cancel(thread_cons_odd);
    test_destroy();
};

// TEST5
// prod_even and prod_odd are running
// Should produce 20 numbers
void test_5() {
    printf("Test 5\n");
    test_init(21);
    pthread_t thread_prod_even;
    pthread_t thread_prod_odd;
    pthread_create(&thread_prod_even, NULL, prod_even, NULL);
    pthread_create(&thread_prod_odd, NULL, prod_odd, NULL);
    sleep(1);
    if (fifo_count(&fifo) != 20) {
        printf("FAILED, %d != 20\n", fifo_count(&fifo));
    } else {
        printf("OK\n");
    }
    pthread_cancel(thread_prod_even);
    pthread_cancel(thread_prod_odd);
    test_destroy();
};

/* ---------------------------------- Main ---------------------------------- */

int main(void) {
    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
}
