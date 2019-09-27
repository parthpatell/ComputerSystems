#include "mypthread.h"
// #include <ucontext.h>

#define SSIZE 4096 //size of stack

Scheduler *threadsched;
threadblock *main_thread;
threadblock Main;

ucontext_t main_context;

unsigned int t_id = 1;
int initializevar = 0;						//used in mypthread_create function 

void cust_sig_handler(int signum){			//signal handler for the alarm in the timer function
	mypthread_yield(); 
	signal(SIGVTALRM, cust_sig_handler);	//reinitialize signal so that it can be recieved again in the future
}

void cust_timer_init(){
	signal(SIGVTALRM, cust_sig_handler);	//counts the amount of time a process uses the processor for 

	timer1.it_interval.tv_sec = 0;
	timer1.it_value.tv_sec = 0; 
	timer2.it_interval.tv_sec = 0;
        timer2.it_value.tv_sec = 0;
	timer3.it_interval.tv_sec = 0;
        timer3.it_value.tv_sec = 0;
	
	setitimer(ITIMER_VIRTUAL, &timer1, NULL);	//start timer
}

void cust_scheduler_init(){					//functions to implement thread scheduler based on priority given
	threadsched = (Scheduler *) malloc(sizeof(Scheduler));
	getcontext(&main_context);
	Main.thread_context = main_context;
        Main.thread_context.uc_link = NULL;
	threadsched -> main_thread = &Main;
	threadsched -> main_thread -> thread_status = RUNNING;
	threadsched -> main_thread -> thread_id = 0;
		
	/* initialize queues */
	threadsched -> run_wait_line1 = (waitLine *) malloc(sizeof(waitLine));
	threadsched -> run_wait_line2 = (waitLine *) malloc(sizeof(waitLine));
	threadsched -> run_wait_line3 = (waitLine *) malloc(sizeof(waitLine));
	threadsched -> exit_queue = (waitLine *) malloc(sizeof(waitLine));
	
	/* create queues */
	threadsched -> run_wait_line1 = createLine();
	threadsched -> run_wait_line2 = createLine();
	threadsched -> run_wait_line3 = createLine();
	threadsched -> exit_queue = createLine(); 
	
	main_thread = threadsched -> main_thread;
	threadsched -> current_thread = threadsched -> main_thread;
}


void cust_scheduler_handler(){
	threadblock = (threadblock *) malloc(sizeof(threadblock));
    threadblock *temp_thread = threadsched->current_thread;

	if(temp_thread != main_thread){
	        switch (temp_thread -> thread_status){
		case YIELD:
			if(threadsched->current_thread->thread_priority < 3){		//decrease thread priority
			        threadsched->current_thread -> thread_priority++;
			}
			cust_scheduler_add(threadsched -> current_thread);
		}
	}
		      
	//search for next thread to run by thread priority 
	if(threadsched->run_wait_line1 -> size != 0 && threadsched->run_wait_line1->head->thread_block != temp_thread){
	        threadsched->current_thread = (dequeue((threadsched->run_wait_line1))->thread_block);
		threadsched->current_wait_number = 1;
	  
	} else if (threadsched->run_wait_line2 -> size != 0 && threadsched->run_wait_line2->head->thread_block != temp_thread){
	        threadsched->current_thread = (dequeue((threadsched->run_wait_line2))->thread_block);
		threadsched->current_wait_number = 2;
	  
	} else if (threadsched->run_wait_line3 -> size != 0 && threadsched->run_wait_line3->head->thread_block != temp_thread){
	        threadsched->current_thread = (dequeue((threadsched->run_wait_line3))->thread_block);
		threadsched->current_wait_number = 3;
	  
	} else {
	        threadsched->current_thread = main_thread;
		threadsched->current_wait_number = 1;
	}
		
	//swaps new threads context             
	if(threadsched->current_thread == main_thread && temp_thread == main_thread){
	        return;
	}

	//decide how long to let the thread execute for depending on the time it needs the processor for 
	switch(threadsched->current_wait_number){
	  
	case 1:
	        setitimer(ITIMER_VIRTUAL, &timer1, NULL);
		break;

	case 2:
	        setitimer(ITIMER_VIRTUAL, &timer2, NULL);
		break;
	case 3:
			setitimer(ITIMER_VIRTUAL, &timer3, NULL);
		break;
	default:
	        return;
	}
	swapcontext(&(temp_thread -> thread_context), &(threadsched->current_thread -> thread_context));
	
}

void cust_scheduler_add(threadblock *block){			//add to scheduler queue line depending on thread priority
	switch(block -> thread_priority){
		case 0:
			enqueue((threadsched -> exit_queue), block);
			break;
		case 1:
			enqueue((threadsched -> run_wait_line1), block); 		
			break;
		case 2:
			enqueue((threadsched -> run_wait_line2), block);
			break;
		case 3:
			enqueue((threadsched -> run_wait_line3), block);
			break;
	}
}

											//these functions implement the 'waiting line' of threads to be executed through
											//a node ordered structure
Node *createNode(threadblock *block){
	Node *node = (Node *) malloc(sizeof(Node));
	node -> thread_block = block;	
	node -> next = NULL;
	return node;
}

waitLine *createLine(){
	waitLine *x = (waitLine *) malloc (sizeof(waitLine));
	x -> head = NULL;
	x -> tail = NULL;
	x -> size = 0;
	return x;
}

void enqueue(waitLine *x, threadblock *block){
	Node *node = createNode(block);
	
	if(x -> tail == NULL){	//if waiting line is empty
		x -> head = node;
		x -> tail = node;
	}else{
		x -> tail -> next = node;
		x -> tail = node;
	}
	x -> size++;
}

Node *dequeue(waitLine *x){
	if(x -> head == NULL){	//if waiting line is empty
		return NULL;
	}
	
	Node *node = x -> head;
	x -> head = x -> head -> next;
	
	if(x -> head == NULL){	//if the head of the line is NULL, then the tail of the line must also be NULL
		x -> tail = NULL;
	}
	x -> size--;
	return node;
}

Node *exitCheck(mypthread_t thread){	//check if a thread has completed executing 
	Node *placeholder;
	for(placeholder = threadsched -> exit_queue -> head; placeholder != NULL; placeholder = placeholder -> next){
		if(placeholder -> thread_block -> thread_id  == thread){
			return placeholder;
		}
	}
	return NULL;
}

int mypthread_create(mypthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg){
	if(initializevar == 0){ 						//this is if this is the first thread being created, must initialize
	        initializevar = 1;						//the scheduler and handler, else, it just goes straight to the thread
			cust_scheduler_init();					//control block
	        cust_timer_init();
		threadsched->current_thread = main_thread; //sets the new thread to be the 'main thread' in the scheduler
	}
													//creates new thread
	threadblock *next_thread = (threadblock *)malloc(sizeof(threadblock));
	getcontext(&(next_thread -> thread_context));
	next_thread -> thread_status = READY;			//set status of thread to 'READY', so that it can execute
	next_thread -> thread_priority = 1;				//set priority to 1 to execute!
	next_thread -> thread_id = t_id;				
	*thread = next_thread -> thread_id;
	next_thread -> thread_context.uc_stack.ss_size = SSIZE;
	next_thread -> thread_context.uc_stack.ss_sp = malloc(SSIZE);
	next_thread -> thread_context.uc_link = &main_context;
	t_id++;
	
													//makes context and passes arguments to the function being executed
	makecontext(&(next_thread -> thread_context), (void *) function, 1, arg);
	
													//add new thread to scheduler, and call the scheduler handler
	if(threadsched->current_thread != main_thread){
		threadsched -> current_thread -> thread_status = YIELD;	
	}
	cust_scheduler_add(next_thread);
	cust_scheduler_handler();
	return 0;
}

int mypthread_yield(){
	threadsched-> current_thread -> thread_status = YIELD;	//give CPU to other threads
	cust_scheduler_handler();
	return 0;
}

void mypthread_exit(void *val_ptr){
	threadsched -> current_thread -> thread_status = KILLED;	//terminate the thread 	
    threadsched -> current_thread -> return_value = val_ptr;
	threadsched -> current_thread -> thread_priority = 0;		//priority of 0 does not allow a thread to execute
	cust_scheduler_add(threadsched -> current_thread);
	cust_scheduler_handler(); 
}

int mypthread_join(mypthread_t thread, void **val_ptr){
	Node *placeholder = exitCheck(thread);				//performs exitcheck of the thread
	while(placeholder == NULL);							//waits for thread to end 
	
	if(val_ptr){
		*val_ptr = placeholder -> thread_block -> return_value;
	}
	return 0;	
}


