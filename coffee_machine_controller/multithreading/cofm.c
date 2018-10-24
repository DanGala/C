#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include "fsm.h"
#include "task.h"

/******************* Constants *******************/

#define PRICE 50					//Coffe price
#define FLAG_BUTTON 0x01			//Flag indicating that the init button has been pressed
#define FLAG_ENABLE 0x02			//Flag indicating that the coffee service has been enabled
#define FLAG_RETURN 0x04			//Flag indicating that cash return is pending
#define T1 20000000					//Activation period of task 1 (cash handler)
#define T2 50000000					//Activation period of task 2 (user interface)
#define T3 100000000				//Activation period of task 3 (coffee service)

/******************* FSMs states *******************/

enum cofm_state {				//Coffe serving states
	COFM_WAITING,				//Waiting for init
	COFM_CUP,					//Serving cup
	COFM_COFFEE,				//Serving coffee
	COFM_MILK,					//Serving milk
};

enum cash_state {					//Cash handling states 
	CASH_WAITING,					//Waiting for minimum credit entry
	CASH_ADQUIRED,					//Minimum credit has been entered
};

/******************* Global variables *******************/

static int flags = 0;					//Status flags register
static int credit = 0;					//Available credit
pthread_mutex_t lock_flags;				//Mutex that controls access to the shared variable 'flags'
pthread_mutex_t lock_credit;				//Mutex that controls access to the shared variable 'credit'


/******************* FSMs guard functions *******************/

static int button_pressed (fsm_t* this){			//Guard function that checks if coffee service must begin
	pthread_mutex_lock(&lock_flags);
	int pressed = ((flags&FLAG_ENABLE)*(flags&FLAG_BUTTON));	//which must happen if the button is pressed while service is enabled
	pthread_mutex_unlock(&lock_flags);
	return pressed;
}

static int next (fsm_t* this){			//Guard function that simulates a timed triggering
	return 1;				            
}

static int price_reached (fsm_t* this){	//Guard function that checks if coffee service must be enabled
	int reached = 0;
	pthread_mutex_lock(&lock_credit);
	pthread_mutex_lock(&lock_flags);
	if(credit >= PRICE){				//which must happen if enough credit has been entered
		flags |= FLAG_ENABLE;
		reached = 1;
	} 
	pthread_mutex_unlock(&lock_credit);
	pthread_mutex_unlock(&lock_flags);
	return reached;					
}

static int return_pressed (fsm_t* this){		//Guard function that checks if cash must be returned
	pthread_mutex_lock(&lock_flags);
	int pressed = (flags & FLAG_RETURN);
	pthread_mutex_unlock(&lock_flags);
	return pressed;
}

/******************* FSMs output functions *******************/

static void notify (fsm_t* this){			//Output functions that enables the coffee service and notifies the user
	printf("\nCoffee service is enabled\n\n");
}

static void give_change (fsm_t* this){			//Output function that triggers change giving
	pthread_mutex_lock(&lock_flags);
	pthread_mutex_lock(&lock_credit);
	flags = 0;					//When change giving is forced by the user, available credit is nuled 
	printf("Giving %d pennies\n\n", credit);	//and coffe service is disabled to avoid free coffee service.
	credit = 0;
	pthread_mutex_unlock(&lock_flags);
	pthread_mutex_unlock(&lock_credit);
}

static void cup (fsm_t* this){			//Output function of the first coffee service state transition
	pthread_mutex_lock(&lock_flags);
	pthread_mutex_lock(&lock_credit);
	credit -= PRICE;					//Coffee price is deduced from the available credit,
	flags &= ~(FLAG_BUTTON);			//Both flags indicating button pressed and service enabled are cleared
	flags &= ~(FLAG_ENABLE);
	pthread_mutex_unlock(&lock_flags);
	pthread_mutex_unlock(&lock_credit);
	printf("LED switches off.\n");			//LED is switched off to indicate that the machine is busy serving
	printf("Dropping cup.\n\n");			//and the cup is dropped
}

static void coffee (fsm_t* this){			//Output function of the second coffee service state transition
	printf("Serving coffee.\n\n");			//Coffee is served
}

static void milk (fsm_t* this){				//Output function of the third coffee service state transition
	printf("Seving milk.\n\n");				//Milk is served
}

static void finish (fsm_t* this){			//Output function of the last coffee service state transition
	pthread_mutex_lock(&lock_flags);
	printf("Coffee is ready.\n");			//Coffee is ready
	printf("LED switches on.\n\n");			//LED is switched on to indicate that the machine is ready to serve another coffee
	flags |= FLAG_RETURN;					//Change is returned, if there's any
	pthread_mutex_unlock(&lock_flags);
}

// Explicit FSM description for coffee service behaviour
static fsm_trans_t cofm[] = {					
	{ COFM_WAITING, button_pressed, COFM_CUP, cup },	
	{ COFM_CUP, next, COFM_COFFEE, coffee },		
	{ COFM_COFFEE, next, COFM_MILK, milk },		
	{ COFM_MILK, next, COFM_WAITING, finish },		
	{ -1, NULL, -1, NULL },					
};
// Explicit FSM description for cash handling behaviour
static fsm_trans_t cash[] = {						
	{ CASH_WAITING, price_reached, CASH_ADQUIRED, notify },		
	{ CASH_WAITING, return_pressed, CASH_WAITING, give_change },	
	{ CASH_ADQUIRED, return_pressed, CASH_WAITING, give_change },	
	{ -1, NULL, -1, NULL},						
};

/******************* Thread start routines *******************/ 

static void * fireCash(){			//Thread init function for cash handling
	fsm_t* cash_fsm = fsm_new(cash);

/*	FILE *file_timing;
	file_timing = fopen("./computeTime1.txt","a");
	if(!file_timing){
		printf("Error opening computeTime1.txt\n");
		pthread_exit(NULL);
	}
*/
	struct timespec t_before, t_after, t_fill, t_remain;
	long difNanos;
	t_fill.tv_sec = 0;
        while(1){
		clock_gettime(CLOCK_MONOTONIC, &t_before);
		fsm_fire(cash_fsm);
		clock_gettime(CLOCK_MONOTONIC, &t_after);
		difNanos = t_after.tv_nsec - t_before.tv_nsec;
		if(difNanos>=0){
			t_fill.tv_nsec = T3 - ((long)((t_after.tv_sec - t_before.tv_sec)*1000000000)+difNanos);
//			fprintf(file_timing, "%ld ns\n", t_fill.tv_nsec);
		} else {
			t_fill.tv_nsec = T3 - ((long)((t_after.tv_sec - t_before.tv_sec - 1)*1000000000)-difNanos);
//			fprintf(file_timing, "%ld ns\n", t_fill.tv_nsec);
		}
		if(nanosleep(&t_fill,&t_remain)<0) printf("Error of fill in task 1\n");
	}
//	fclose(file_timing);
	pthread_exit(NULL);
}

static void * userEntries(){			//Thread init function for user interface management
	fcntl(0,F_SETFL, fcntl(0,F_GETFL)|O_NONBLOCK);  //Make the scanf function non-blocking

/*      FILE *file_timing;                      
        file_timing = fopen("./computeTime2.txt","a");
        if(!file_timing){
                printf("Error opening computeTime2.txt\n");
		pthread_exit(NULL);
        }
*/
	int input;
	long difNanos;
	struct timespec t_before, t_after, t_fill, t_remain;
	t_fill.tv_sec = 0;
	while(1){
		clock_gettime(CLOCK_MONOTONIC, &t_before);

		if(scanf("%d",&input)==1){
       		 	pthread_mutex_lock(&lock_flags);
   	        	flags &= ~(FLAG_RETURN);              //By default, give_change flag is off 
   	        	flags &= ~(FLAG_BUTTON);              //By default, button_pressed flag is off
   	        	if(input>1){                          //Integers greater that 1 symbolize credit entry
   	        		pthread_mutex_lock(&lock_credit);
   	                credit += input;
   	                pthread_mutex_unlock(&lock_credit);
   	        	}else if(input==0){                   //A 'zero' symbolizes pressing the return button
   	                flags |= FLAG_RETURN;
   	        	}else if(input==1){                   //A 'one' symbolizes pressing the init button
   	                flags |= FLAG_BUTTON;
   	        	}else{
					pthread_mutex_unlock(&lock_flags);
					pthread_exit(NULL);
				}
   	        	pthread_mutex_unlock(&lock_flags);
		}
		clock_gettime(CLOCK_MONOTONIC, &t_after);
		difNanos = t_after.tv_nsec - t_before.tv_nsec;
		if(difNanos >= 0){
			t_fill.tv_nsec = T1 - ((long)((t_after.tv_sec - t_before.tv_sec)*1000000000) + difNanos);
//			fprintf(file_timing, "%ld ns\n", t_fill.tv_nsec);
		} else {
			t_fill.tv_nsec = T1 - ((long)((t_after.tv_sec - t_before.tv_sec - 1)*1000000000) - difNanos);
//			fprintf(file_timing, "%ld ns\n", t_fill.tv_nsec);
		}
		if(nanosleep(&t_fill,&t_remain)<0) printf("Error of fill in task 2\n");
	}
//	fclose(file_timing);
	pthread_exit(NULL);
}

static void * fireCofm(){			//Thread init function for coffee service management
	fsm_t* cofm_fsm = fsm_new(cofm);

/*	FILE *file_timing;
	file_timing = fopen("./computeTime3.txt","a");
	if(!file_timing){
		printf("Error opening computeTime3.txt\n");
		pthread_exit(NULL);
	}
*/
	struct timespec t_before, t_after, t_fill, t_remain;
	long difNanos;
	t_fill.tv_sec = 0;
        while(1){
		clock_gettime(CLOCK_MONOTONIC, &t_before);
		fsm_fire(cofm_fsm);
		clock_gettime(CLOCK_MONOTONIC, &t_after);
		difNanos = t_after.tv_nsec - t_before.tv_nsec;
		if(difNanos >= 0){
			t_fill.tv_nsec = T2 - ((long)((t_after.tv_sec - t_before.tv_sec)*1000000000)+difNanos);
//			fprintf(file_timing, "%ld ns\n", t_fill.tv_nsec);
		} else {
			t_fill.tv_nsec = T2 - ((long)((t_after.tv_sec - t_before.tv_sec - 1)*1000000000)-difNanos);	
//			fprintf(file_timing, "%ld ns\n", t_fill.tv_nsec);
		}
		if(nanosleep(&t_fill,&t_remain)<0) printf("Error of fill in task 3\n");
	}
//	fclose(file_timing);
	pthread_exit(NULL);
}

/******************* Main programm *******************/ 

int main () {

	mutex_init(&lock_flags, 0);		//Mutex initialization
	mutex_init(&lock_credit, 0);	//using priority inheritance

	printf("Enter the minimum credit and press the button to start the service\n");

	task_new("task1", fireCash, 3);						//Create 3 threads: one to handle de credit,
	pthread_t tid2 = task_new("task2", userEntries, 2);	//another one to interact with the user
	task_new("task3", fireCofm, 1);						//and finally, one to serve the coffee

	pthread_join(tid2, NULL);		//Wait for all task to finish before termination

	return 0;
}
