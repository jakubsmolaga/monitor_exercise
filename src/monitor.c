#include <monitor.h>

/* -------------------------------------------------------------------------- */
/* -------------------------------- Condition ------------------------------- */
/* -------------------------------------------------------------------------- */

void cond_init(cond_t* cond) {
    cond->wait_count = 0;
    sem_init(&cond->sem, 0, 0);
};

void cond_destroy(cond_t* cond) {
    sem_destroy(&cond->sem);
};

void cond_wait(cond_t* cond) {
    cond->wait_count++;
    sem_wait(&cond->sem);
};

int cond_signal(cond_t* cond) {
    if (cond->wait_count == 0)
        return 0;
    cond->wait_count--;
    sem_post(&cond->sem);
    return 1;
};

/* -------------------------------------------------------------------------- */
/* --------------------------------- Monitor -------------------------------- */
/* -------------------------------------------------------------------------- */

void monitor_init(monitor_t* monitor) {
    sem_init(&monitor->mut, 0, 1);
};

void monitor_destroy(monitor_t* monitor) {
    sem_destroy(&monitor->mut);
};

void monitor_enter(monitor_t* monitor) {
    sem_wait(&monitor->mut);
};

void monitor_leave(monitor_t* monitor) {
    sem_post(&monitor->mut);
};

void monitor_wait(monitor_t* monitor, cond_t* cond) {
    monitor_leave(monitor);
    cond_wait(cond);
};

int monitor_signal(monitor_t* monitor, cond_t* cond) {
    return cond_signal(cond);
};