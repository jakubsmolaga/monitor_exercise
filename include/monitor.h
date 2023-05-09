#pragma once

#include <semaphore.h>

/* -------------------------------------------------------------------------- */
/* -------------------------------- Condition ------------------------------- */
/* -------------------------------------------------------------------------- */

// Condition variable
typedef struct {
    int wait_count;  // Number of threads waiting on the condition variable
    sem_t sem;       // Semaphore for the condition variable
} cond_t;

// Initialize a condition variable
void cond_init(cond_t*);

// Destroy a condition variable
void cond_destroy(cond_t*);

// Wait on a condition variable
void cond_wait(cond_t*);

// Signal a condition variable
int cond_signal(cond_t*);

/* -------------------------------------------------------------------------- */
/* --------------------------------- Monitor -------------------------------- */
/* -------------------------------------------------------------------------- */

// Monitor
typedef struct {
    sem_t mut;  // Mutex for the monitor
} monitor_t;

// Initialize a monitor
void monitor_init(monitor_t*);

// Destroy a monitor
void monitor_destroy(monitor_t*);

// Enter a monitor
void monitor_enter(monitor_t*);

// Leave a monitor
void monitor_leave(monitor_t*);

// Wait on a condition variable
void monitor_wait(monitor_t*, cond_t*);

// Signal a condition variable
int monitor_signal(monitor_t*, cond_t*);