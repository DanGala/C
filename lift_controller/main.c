#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include "fsm.h"

#define DOOR_TIME	5000         //             - Door's timeout, T
#define MOVE_TIME 5000         //             - Simulates travel delay between floors 
#define FLAG_PANNEL_0   0x01   //000000000001
#define FLAG_PANNEL_1   0x02   //000000000010
#define FLAG_PANNEL_2   0x04   //000000000100
#define FLAG_FLOOR_0    0x08   //000000001000
#define FLAG_FLOOR_1    0x10   //000000010000
#define FLAG_FLOOR_2    0x20   //000000100000
#define FLAG_SENSOR_0   0x40   //000001000000
#define FLAG_SENSOR_1   0x80   //000010000000
#define FLAG_SENSOR_2   0x100  //000100000000
#define FLAG_DOOR       0x200  //001000000000 - Door's opened when active, closed otherwise
#define FLAG_TIMER_DOOR 0x400  //010000000000 - Door's open-time is over when active
#define FLAG_TIMER_MOVE 0x800  //100000000000 - Travel between floors is over when active

enum lift_state {
  FLOOR0,
  FLOOR1,
  FLOOR2,
};

enum door_state {
  OPENED,
  CLOSED,
};

static uint16_t flags = FLAG_SENSOR_0; //Initial state: door's closed at floor 0, no pending calls.

static void timer_door_isr (union sigval arg)
{
  flags |= FLAG_TIMER_DOOR;
}

static void timer_door_start (int ms)
{
  timer_t timerid;
  struct itimerspec spec;
  struct sigevent se;
  se.sigev_notify = SIGEV_THREAD;
  se.sigev_value.sival_ptr = &timerid;
  se.sigev_notify_function = timer_door_isr;
  se.sigev_notify_attributes = NULL;
  spec.it_value.tv_sec = ms / 1000;
  spec.it_value.tv_nsec = (ms % 1000) * 1000000;
  spec.it_interval.tv_sec = 0;
  spec.it_interval.tv_nsec = 0;
  timer_create (CLOCK_REALTIME, &se, &timerid);
  timer_settime (timerid, 0, &spec, NULL);
}

static void timer_move_isr (union sigval arg)
{
  flags |= FLAG_TIMER_MOVE;
}

static void timer_move_start (int ms)
{
  timer_t timerid;
  struct itimerspec spec;
  struct sigevent se;
  se.sigev_notify = SIGEV_THREAD;
  se.sigev_value.sival_ptr = &timerid;
  se.sigev_notify_function = timer_move_isr;
  se.sigev_notify_attributes = NULL;
  spec.it_value.tv_sec = ms / 1000;
  spec.it_value.tv_nsec = (ms % 1000) * 1000000;
  spec.it_interval.tv_sec = 0;
  spec.it_interval.tv_nsec = 0;
  timer_create (CLOCK_REALTIME, &se, &timerid);
  timer_settime (timerid, 0, &spec, NULL);
}

static int arrived0 (fsm_t* this)
{
  return (flags&(FLAG_TIMER_MOVE));
}

static int arrived1 (fsm_t* this)
{
  return (flags&(FLAG_TIMER_MOVE));
}

static int arrived2 (fsm_t* this)
{
  return (flags&(FLAG_TIMER_MOVE));
}

static int ascend0 (fsm_t* this)
{
  // (sensor0 AND (NOT door) AND (floor1 OR floor2 OR pannel1 OR pannel2))
  return ((flags&FLAG_SENSOR_0)&&(!(flags&FLAG_DOOR))&&(flags&(FLAG_FLOOR_1|FLAG_FLOOR_2|FLAG_PANNEL_1|FLAG_PANNEL_2)));
}

static int ascend1 (fsm_t* this)
{
  // (sensor1 AND (NOT door) AND (floor2 OR pannel2))
  return ((flags&FLAG_SENSOR_1)&&(!(flags&FLAG_DOOR))&&(flags&(FLAG_FLOOR_2|FLAG_PANNEL_2)));
}

static int descend1 (fsm_t* this)
{
  // (sensor1 AND (NOT door) AND (floor0 OR pannel0))
  return ((flags&FLAG_SENSOR_1)&&(!(flags&FLAG_DOOR))&&(flags&(FLAG_FLOOR_0|FLAG_PANNEL_0)));
}

static int descend2 (fsm_t* this)
{
  // (sensor2 AND (NOT door) AND (floor0 OR floor1 OR pannel0 OR pannel1))
  return ((flags&FLAG_SENSOR_2)&&(!(flags&FLAG_DOOR))&&(flags&(FLAG_FLOOR_0|FLAG_FLOOR_1|FLAG_PANNEL_0|FLAG_PANNEL_1)));
}

static int keepOpen (fsm_t* this)
{
  // (sensorX AND (floorX OR pannelX))
  return (((flags&FLAG_SENSOR_0)&&(flags&(FLAG_FLOOR_0|FLAG_PANNEL_0)))||((flags&FLAG_SENSOR_1)&&(flags&(FLAG_FLOOR_1|FLAG_PANNEL_1)))||((flags&FLAG_SENSOR_2)&&(flags&(FLAG_FLOOR_2|FLAG_PANNEL_2))));
}

static int mustOpen (fsm_t* this)
{
  // (sensorX AND (floorX OR pannelX))
  return (((flags&FLAG_SENSOR_0)&&(flags&(FLAG_FLOOR_0|FLAG_PANNEL_0)))||((flags&FLAG_SENSOR_1)&&(flags&(FLAG_FLOOR_1|FLAG_PANNEL_1)))||((flags&FLAG_SENSOR_2)&&(flags&(FLAG_FLOOR_2|FLAG_PANNEL_2))));
}

static int mustClose (fsm_t* this)
{
  // (timer_finished OR (sensorX AND (pannelY OR pannelZ)))
  return ((flags&FLAG_TIMER_DOOR)||(((flags&FLAG_SENSOR_0)&&(flags&(FLAG_PANNEL_1|FLAG_PANNEL_2)))||((flags&FLAG_SENSOR_1)&&(flags&(FLAG_PANNEL_0|FLAG_PANNEL_2)))||((flags&FLAG_SENSOR_2)&&(flags&(FLAG_PANNEL_0|FLAG_PANNEL_1)))));
}

static void ascending (fsm_t* this)
{
  flags &= ~(FLAG_SENSOR_0|FLAG_SENSOR_1|FLAG_SENSOR_2);
  timer_move_start(MOVE_TIME);
}

static void descending (fsm_t* this)
{
  flags &= ~(FLAG_SENSOR_0|FLAG_SENSOR_1|FLAG_SENSOR_2);
  printf("Descending.\n");
  timer_move_start(MOVE_TIME);
}

static void setFloor0 (fsm_t* this)
{
  flags &= ~(FLAG_TIMER_MOVE);
  flags |= FLAG_SENSOR_0;
}

static void setFloor1 (fsm_t* this)
{
  flags &= ~(FLAG_TIMER_MOVE);
  flags |= FLAG_SENSOR_1;
}

static void setFloor2 (fsm_t* this)
{
  flags &= ~(FLAG_TIMER_MOVE);
  flags |= FLAG_SENSOR_2;
}

static void openDoor (fsm_t* this)
{
  flags |= FLAG_DOOR;
  if (flags&FLAG_SENSOR_0){
    printf("Stopped at floor 0.\n");
    flags &= ~(FLAG_FLOOR_0|FLAG_PANNEL_0);
  } else if (flags&FLAG_SENSOR_1){
    printf("Stopped at floor 1.\n");
    flags &= ~(FLAG_FLOOR_1|FLAG_PANNEL_1);
  } else {
    printf("Stopped at floor 2.\n");
    flags &= ~(FLAG_FLOOR_2|FLAG_PANNEL_2);
  }
  printf("Opening door.\n");
  flags &= ~(FLAG_TIMER_DOOR);
  timer_door_start(DOOR_TIME);
}

static void closeDoor (fsm_t* this)
{
  printf("Closing door.\n");
  flags &= ~FLAG_DOOR;
}

// Explicit FSM description
static fsm_trans_t lift[] = {
  { FLOOR0, ascend0, FLOOR1, ascending },
  { FLOOR0, arrived0, FLOOR0, setFloor0 },
  { FLOOR1, ascend1, FLOOR2, ascending },
  { FLOOR1, descend1, FLOOR0, descending },
  { FLOOR1, arrived1, FLOOR1, setFloor1 },
  { FLOOR2, descend2, FLOOR1, descending },
  { FLOOR2, arrived2, FLOOR2, setFloor2 },
  {-1, NULL, -1, NULL },
};

static fsm_trans_t door[] = {
  { OPENED, keepOpen, OPENED, openDoor },
  { OPENED, mustClose, CLOSED, closeDoor },
  { CLOSED, mustOpen, OPENED, openDoor },
  {-1, NULL, -1, NULL },
};

// Utility functions, should be elsewhere

// res = a - b
void
timeval_sub (struct timeval *res, struct timeval *a, struct timeval *b)
{
  res->tv_sec = a->tv_sec - b->tv_sec;
  res->tv_usec = a->tv_usec - b->tv_usec;
  if (res->tv_usec < 0) {
    --res->tv_sec;
    res->tv_usec += 1000000;
  }
}

// res = a + b
void
timeval_add (struct timeval *res, struct timeval *a, struct timeval *b)
{
  res->tv_sec = a->tv_sec + b->tv_sec
    + a->tv_usec / 1000000 + b->tv_usec / 1000000; 
  res->tv_usec = a->tv_usec % 1000000 + b->tv_usec % 1000000;
}

// wait until next_activation (absolute time)
void delay_until (struct timeval* next_activation)
{
  struct timeval now, timeout;
  gettimeofday (&now, NULL);
  timeval_sub (&timeout, next_activation, &now);
  select (0, NULL, NULL, NULL, &timeout);
}



int main ()
{
  struct timeval clk_period = { 0, 250000 };
  struct timeval next_activation;

  fsm_t* lift_fsm = fsm_new (lift);
  fsm_t* door_fsm = fsm_new (door);
  char entry;

  fcntl(0,F_SETFL, fcntl(0,F_GETFL)|O_NONBLOCK); //Set scanf in non-blocking mode
    
  gettimeofday (&next_activation, NULL);
  while (1){
    if(scanf("%c", &entry)==1){
      switch (entry){
        case 'q': flags |= FLAG_PANNEL_0; break;
        case 'w': flags |= FLAG_PANNEL_1; break;
        case 'e': flags |= FLAG_PANNEL_2; break;
        case 'r': flags |= FLAG_FLOOR_0; break;
        case 't': flags |= FLAG_FLOOR_1; break;
        case 'y': flags |= FLAG_FLOOR_2; break;
        default: break;
      }
    }
    fsm_fire (door_fsm);
    fsm_fire (lift_fsm);
    timeval_add (&next_activation, &next_activation, &clk_period);
    delay_until (&next_activation);
  }
  return 1;
}
