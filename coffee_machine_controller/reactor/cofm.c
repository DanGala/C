#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include "fsm.h"
#include "reactor.h"

/******************* Constants *******************/

#define PRICE 50				//Coffe price
#define FLAG_BUTTON 0x01		//Flag indicating that the init button has been pressed
#define FLAG_ENABLE 0x02		//Flag indicating that the coffee service has been enabled
#define FLAG_RETURN 0x04		//Flag indicating that cash return is pending
#define T1 500					//Activation period of task 1 (cash handler)
#define T2 500					//Activation period of task 2 (user interface)
#define T3 500					//Activation period of task 3 (coffee service)

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

/******************* Shared Resources *******************/

static int flags = 0;					//Status flags register
static int credit = 0;					//Available credit


/******************* FSMs guard functions *******************/

static int button_pressed (fsm_t* this){				//Guard function that checks if coffee service must begin
	return ((flags&FLAG_ENABLE)*(flags&FLAG_BUTTON));	//which must happen if the button is pressed while service is enabled
}

static int next (fsm_t* this){	//Guard function that simulates a timed triggering
	return 1;						
}

static int price_reached (fsm_t* this){			//Guard function that checks if coffee service must be enabled
	if(credit >= PRICE){						//which must happen if enough credit has been entered
		return 1;
	}
	return 0;
}

static int return_pressed (fsm_t* this){		//Guard function that checks if cash must be returned
	return (flags & FLAG_RETURN);
}

/******************* FSMs output functions *******************/

static void notify (fsm_t* this){			//Output functions that enables the coffee service and notifies the user
	flags |= FLAG_ENABLE;
	printf("\nCoffee service is enabled\n\n");
}

static void give_change (fsm_t* this){			//Output function that triggers give_change giving
	flags = 0;									//When give_change giving is forced by the user, available credit is nuled 
	printf("Giving %d pennies\n\n", credit);	//and coffe service is disabled to avoid free coffee service.
	credit = 0;
}

static void cup (fsm_t* this){				//Output function of the first coffee service state transition
	credit -= PRICE;						//Coffee price is deduced from the available credit,
	flags &= ~(FLAG_BUTTON);				//Both flags indicating button pressed and service enabled are cleared
	flags &= ~(FLAG_ENABLE);				
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
	printf("Coffee is ready.\n");			//Coffee is ready
	printf("LED switches on.\n\n");			//LED is switched on to indicate that the machine is ready to serve another coffee
	flags |= FLAG_RETURN;					//Change is returned, if there's any
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

/******************* Event Handler routines *******************/ 

static void fireCofm(struct event_handler_t* this){			//Event Handler for coffee machine management

/*	struct timespec t_before, t_after;

	FILE *file_timing; 		                     
        file_timing = fopen("./computeTime3.txt","a");
        if(!file_timing){
                printf("Error opening computeTime3.txt\n");
        }

	long difNanos;

	clock_gettime(CLOCK_MONOTONIC, &t_before);
*/	static const struct timeval period = {0, T3};
	fsm_fire((fsm_t*)(this->user_data));
	timeval_add(&this->next_activation, &this->next_activation, &period);	//Update task's next activation time
/*	clock_gettime(CLOCK_MONOTONIC, &t_after);
	difNanos = t_after.tv_nsec - t_before.tv_nsec;
	if(difNanos >= 0){
		fprintf(file_timing, "%ld ns\n", (long)(t_after.tv_sec-t_before.tv_sec)*1000000000+difNanos);
	} else {
		fprintf(file_timing, "%ld ns\n", (long)(t_after.tv_sec-t_before.tv_sec-1)*1000000000-difNanos);
	}
	fclose(file_timing); */
}

static void fireCash(struct event_handler_t* this){			//Event Handler for cash handling

/*	struct timespec t_before, t_after;

	FILE *file_timing;     		                
        file_timing = fopen("./computeTime1.txt","a");
        if(!file_timing){
                printf("Error openig computeTime1.txt\n");
        }
	long difNanos;

	clock_gettime(CLOCK_MONOTONIC,&t_before);
*/	static const struct timeval period = {0, T1};
	fsm_fire((fsm_t*)(this->user_data));
	timeval_add(&this->next_activation, &this->next_activation, &period);	//Update task's next activation time
/*	clock_gettime(CLOCK_MONOTONIC, &t_after);
	difNanos = t_after.tv_nsec - t_before.tv_nsec;
	if(difNanos>=0){
		fprintf(file_timing, "%ld ns\n", (long)(t_after.tv_sec-t_before.tv_sec)*1000000000+difNanos);
	} else {
		fprintf(file_timing, "%ld ns\n", (long)(t_after.tv_sec-t_before.tv_sec-1)*1000000000-difNanos);
	}
	fclose(file_timing); */
}

static void userEntries(struct event_handler_t* this){		//Event Handler for user interface management

/*	struct timespec t_before, t_after;

	FILE *file_timing;				
	file_timing = fopen("./computeTime2.txt","a");
	if(!file_timing){
		printf("Error abriendo computeTime2.txt\n");
	}
	long difNanos;
	clock_gettime(CLOCK_MONOTONIC, &t_before);
*/
	fcntl(0, F_SETFL, fcntl(0,F_GETFL)|O_NONBLOCK);	//Make the scanf function non-blocking
	static const struct timeval period = {0, T2};
	int input;
	if(scanf("%d",&input)==1){
//		if(fscanf(file_inputs, "%d", &input)==1){
		flags &= ~(FLAG_RETURN);	//By default, give_change flag is off
		flags &= ~(FLAG_BUTTON);	//By default, button_pressed flag is off
		if(input>1){				//Integers greater that 1 symbolize credit entry
			credit += input;
		}else if(input==0){			//A 'zero' symbolizes pressing the return button
			flags |= FLAG_RETURN;
		}else if(input==1){			//A 'one' symbolizes pressing the init button
			flags |= FLAG_BUTTON;
		}else {
			exit(0);				//Negative integers terminate the program
		} 				
    }
	timeval_add(&this->next_activation, &this->next_activation, &period);	//Update task's next activation time
/*	clock_gettime(CLOCK_MONOTONIC, &t_after);
	difNanos = t_after.tv_nsec - t_before.tv_nsec;
	if(difNanos>=0){
		fprintf(file_timing, "%ld ns\n", (long)(t_after.tv_sec-t_before.tv_sec)*1000000000+difNanos);
	} else {
		fprintf(file_timing, "%ld ns\n", (long)(t_after.tv_sec-t_before.tv_sec-1)*1000000000-difNanos);
	}
	fclose(file_timing); */
}

/******************* Main programm *******************/ 

int main () {

	fsm_t* cash_fsm = fsm_new(cash);
	fsm_t* cofm_fsm = fsm_new(cofm);

	EventHandler eh1, eh2, eh3;
	reactor_init();

	event_handler_init(&eh1, 3, fireCash, (void*)(cash_fsm));
	event_handler_init(&eh2, 4, userEntries, NULL);
	event_handler_init(&eh3, 1, fireCofm, (void*)(cofm_fsm));

	reactor_add_handler(&eh1);
	reactor_add_handler(&eh2);
	reactor_add_handler(&eh3);

	printf("Enter the minimum credit and press the button to start the service\n");

	while(1){
		reactor_handle_events();
	}
	
	return 0;
}
