#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "task.h"

#define MAXTASKS 20

typedef struct taskdesk_t {
  pthread_t tid;
  const char* name;
  int prio;
} taskdesc_t;

static taskdesc_t desc[MAXTASKS];
static int ntasks = 0;

pthread_t
task_new (const char* name, void *(*f)(void *), int prio)
{
  taskdesc_t* tdesc = &desc[ntasks++];
  pthread_attr_t attr;
  struct sched_param sparam;
  sparam.sched_priority = sched_get_priority_min (SCHED_FIFO) + prio;

  pthread_attr_init (&attr);
  pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);
  pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
  pthread_attr_setschedparam (&attr, &sparam);
  pthread_create (&tdesc->tid, &attr, f, tdesc);

  tdesc->name = name;
  tdesc->prio = prio;

  return tdesc->tid;
}

void
mutex_init (pthread_mutex_t* m, int prioceiling)
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init (&attr);
  
  /* Only for priority ceiling protocol (not in Xenomai) */
  /* pthread_mutexattr_setprotocol (&attr, PTHREAD_PRIO_PROTECT); */

  /* Only for priority inheritance protocol */ 
  pthread_mutexattr_setprotocol (&attr, 1); //El parámetro 1 sustituye al elemento del enumerado de protocolos PTHREAD_PRIO_INHERIT

  
  pthread_mutex_init (m, &attr);

  /* Only for priority ceiling protocol (not in Xenomai) */
  /* pthread_mutex_setprioceiling
    (m, sched_get_priority_min(SCHED_FIFO) + prioceiling, NULL); */
}
