# Monitor exercise
## Description
This repository contains a solution to a simple exercise aimed at demonstrating the use of the `monitor` synchronization primitive as well as `condition variables`.  

## Problem statement
There is a FIFO queue for integers.  
There are 4 types of threads: `prod_even`, `prod_odd`, `cons_even`, `cons_odd`.

- Threads `prod_even` generate even numbers modulo **50** if the queue contains **less than 10 even numbers**.
- Threads `prod_odd` generate odd numbers modulo **50** if the queue contains **less odd numbers than even numbers**.
- Threads `cons_even` consume even numbers from the queue if the queue contains **more than 3 numbers**.
- Threads `cons_odd` consume odd numbers from the queue if the queue contains **more than 7 numbers**.

## What is a condition variable?
A condition variable is a synchronization primitive that allows threads to wait for a certain condition to occur.  
The structure consists of a semaphore and a counter that keeps track of the number of threads waiting for the condition to occur.  
The semaphore is used to block the threads that are waiting for the condition to occur.  
In this implementation the condition variable data structure looks like this:
```c
// Condition variable
typedef struct {
    int wait_count;  
    sem_t sem;
} cond_t;
```
And implements the following functionality:
```c
// Wait on a condition variable
void cond_wait(cond_t*);

// Signal a condition variable
int cond_signal(cond_t*);
```
The `cond_wait` function blocks the thread that calls it and increments the wait count.  
The `cond_signal` function decrements the wait count and unblocks a thread that is waiting on the condition variable.

## What is a monitor?
A monitor is a synchronization primitive that allows to protect a shared resource by allowing only one thread to access it at a time.  
The monitor holds a mutex that is used to block the threads that are trying to access the shared resource.  
In this implementation the monitor data structure looks like this:
```c
// Monitor
typedef struct {
    sem_t mut;  // Mutex for the monitor
} monitor_t;
```
And implements the following functionality:
```c
// Enter a monitor
void monitor_enter(monitor_t*);

// Leave a monitor
void monitor_leave(monitor_t*);

// Wait on a condition variable
void monitor_wait(monitor_t*, cond_t*);

// Signal a condition variable
int monitor_signal(monitor_t*, cond_t*);
```
The `monitor_enter` function locks the mutex of the monitor.  
The `monitor_leave` function unlocks the mutex of the monitor.  
The `monitor_wait` function blocks the thread that calls it and unlocks the mutex of the monitor.
The `monitor_signal` function unblocks a thread that is waiting on the condition variable.

## Operating on the queue
Operations on the queue follow the following pattern:
```c
monitor_enter(&monitor);
if (!can_perform_the_action())
    monitor_wait(&monitor, &condition_variable);
perform_the_action();
monitor_update();
```
* Lock the monitor.
* Check if the action can be performed.
    * If the action cannot be performed, wait on the condition variable.
* Perform the action.
* Update the monitor.

## Monitor update routine
The monitor update routine is responsible for checking if the condition variables should be signaled.  
The routine is called after every operation on the queue.  
The routine looks like this:
```c
if (can_do_something1() && cond_signal(&condition_variable1))
    return;
if (can_do_something2() && cond_signal(&condition_variable2))
    return;
if (can_do_something3() && cond_signal(&condition_variable3))
    return;
// ...
monitor_leave(&monitor);
```
For every condition:
* Check if the condition can be satisfied.
    * If the condition can be satisfied, signal the condition variable and return.

If no thread was signaled, unlock the monitor.  
> *Note*: We don't unlock the monitor if a thread was signaled. The lock is basically passed to the next thread until there is no more work to be done.

## How to run the program?
This project uses the `CMake` build system.  
To build the project run the following commands:
```bash
mkdir build
cd build
cmake ..
make
```
To run the program execute the following command:
```bash
./monitor_exercise
```