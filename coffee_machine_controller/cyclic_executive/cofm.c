#include <stdio.h>
#include "fsm.h"
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#define PRICE 50					//Coffe price
#define FLAG_BUTTON 0x01			//Flag indicating that the init button has been pressed
#define FLAG_ENABLE 0x02			//Flag indicating that the coffee service has been enabled
#define FLAG_RETURN 0x04			//Flag indicating that cash return is pending
#define TM 8000000					//Cyclic executive major cycle in ns

enum cofm_state {					//Coffe serving states
	COFM_WAITING,					//Waiting for init
	COFM_CUP,						//Serving cup
	COFM_COFFEE,					//Serving coffee
	COFM_MILK,						//Serving milk
};

enum cash_state {					//Cash handling states 
	CASH_WAITING,					//Waiting for minimum credit entry
	CASH_ADQUIRED,					//Minimum credit has been entered
};

static int flags = 0;				//Status flags register
static int credit = 0;				//Available credit

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

static void notify (fsm_t* this){			//Output functions that enables the coffee service and notifies the user
	flags |= FLAG_ENABLE;
	printf("\nCoffee service is enabled\n\n");
}

static void give_change (fsm_t* this){			//Output function that triggers change giving
	flags = 0;									//When change giving is forced by the user, available credit is nuled 
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

int main () {									
	fsm_t* cofm_fsm = fsm_new(cofm);
	fsm_t* cash_fsm = fsm_new(cash);

	int input;			//User inputs (init, give_change, credit entry)
	long difNanos;		//timing variable

	fcntl(0,F_SETFL, fcntl(0,F_GETFL)|O_NONBLOCK);	//Make the scanf function non-blocking

	struct timespec t_before;	
	struct timespec t_after;	
	struct timespec fill;	
	struct timespec remaining;		
	t_before.tv_sec=0;
    t_before.tv_nsec=0;
    t_after.tv_sec=0;
    t_after.tv_nsec=0;


/*	FILE *file_inputs;			
	file_inputs = fopen("./inputs.txt","r");
	if(!file_inputs){
		printf("Error opening the file inputs.txt\n");
		return 1;
	}
	FILE *ptr_file;				 
        ptr_file = fopen("./global.txt","a");   
        if(!ptr_file){				 
                printf("Error opening the file global.txt\n");
                return 1;
        }
*/
	printf("Enter the minimum credit and press the button to start the service\n");

	while(1){
		clock_gettime(CLOCK_MONOTONIC, &t_before);

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
			}else break; 				//Negative integers terminate the program
		}
//		clock_gettime(CLOCK_MONOTONIC, &t_after);

//		clock_gettime(CLOCK_MONOTONIC, &t_before);
		fsm_fire(cofm_fsm);
//		clock_gettime(CLOCK_MONOTONIC, &t_after);

//		clock_gettime(CLOCK_MONOTONIC, &t_before);
		fsm_fire(cash_fsm);		
//		clock_gettime(CLOCK_MONOTONIC, &t_after);

		clock_gettime(CLOCK_MONOTONIC, &t_after);

		difNanos = (t_after.tv_nsec - t_before.tv_nsec);
		if(difNanos >= 0){
//			fprintf(ptr_file,"%d\n",(int)(t_after.tv_sec - t_before.tv_sec)*1000000000+(int)(difNanos));
			fill.tv_sec = 0;
			fill.tv_nsec = TM-((int)(t_after.tv_sec-t_before.tv_sec)*1000000000+(int)(difNanos));
		} else {
//			fprintf(ptr_file,"%d\n",(int)(t_after.tv_sec - t_before.tv_sec)*1000000000+(int)(difNanos));
			fill.tv_sec = 0;
			fill.tv_nsec = TM-((int)(t_after.tv_sec-t_before.tv_sec)*1000000000+(int)(-difNanos));
		}

		if(nanosleep(&fill,&remaining)<0){
			printf("Error of fill\n");
		}
	}
//	fclose(file_inputs);
//	fclose(ptr_file);
	return 0;
}
