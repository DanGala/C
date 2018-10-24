#ifndef TASK_H
#define TASK_H

#include <pthread.h>
#include <time.h>
#include <sys/time.h>

void task_setup (void);

pthread_t task_new (const char* name, void *(*f)(void *), int prio);

void mutex_init (pthread_mutex_t* m, int prioceiling);

#endif
