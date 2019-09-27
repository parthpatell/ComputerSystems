//mypthread.c

#include "mypthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "ucontext.h"

enum state{ACTIVE, BLOCKED, DONE};

typedef struct threadNode{
    
// Type your commands
	ucontext_t* context;
	short tid;
	enum state status;
	int join_bool; 
}Node;

//Global

// Type your globals...
mypthread_t *head;
mypthread_t *temp;
mypthread_t *tail;
int numOfNodes = 0;
int current_tid = 1;
int numOfThreads = 1;
Node* main_thread = NULL;
// Type your own functions (void)

void create_node(Node* core) {//create mypthread given node
	temp = (mypthread_t *) malloc(sizeof(mypthread_t));
	temp->mynode = core;
	temp->tid = core->tid;
	temp->prev = NULL;
	temp->next = NULL;
	numOfNodes++;
}

void enqueue(Node* core) {
	create_node(core);
	if (head == NULL) { //for first 
		head = temp;
		tail = temp;
		//printf("Head created. My tid is %d.\n", temp->tid);
	} else {	//add to end of list
		tail->next = temp;
		temp->prev = tail;
		tail = temp;
		tail->next = head;
		head->prev = tail;
		//printf("Created Thread %d. My prev is %d\n",core->tid, temp->prev->tid);
	}
	
}

mypthread_t* searchThread(short tid) {
	temp = head;
	while (temp != NULL) {
		if (temp->tid == tid)
			return temp;
		else {
			temp = temp->next;
		}
	}
	printf("ERROR\nThread with TID %d could not be found!\n", tid);
	exit(0);
}

mypthread_t* searchNextActiveThread(int threadId) {
	temp = head;

	while (temp != NULL) {
		if (temp->tid == threadId) {
			break;
		} else {
			temp = temp->next;
		}
	}
	if (temp->next != NULL) {
		temp = temp->next;
		while (temp != NULL) {
			if (temp->mynode->status == ACTIVE) {
				return temp;
			} else {
				temp = temp->next;
			}
		}
	} else {
		exit(0);
	}
	printf("No active thread found after TID%d\n", threadId);
	exit(0);
}

// e.g., free up sets of data structures created in your library

    
/*  Write your thread create function here... */
int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr, void *(*start_routine) (void *), void *arg){
	if (main_thread == NULL){ 
		main_thread = (Node *) malloc(sizeof(Node));
		main_thread->tid = (short) numOfThreads++;
		ucontext_t* main_context = (ucontext_t*) malloc(sizeof(ucontext_t));
		main_thread->context = main_context;
		if(getcontext((main_thread->context)) != 0){
			printf("error");
			exit(1);
		}
		main_thread->context->uc_stack.ss_sp = (char*) malloc(sizeof(char) * 16384);
		main_thread->context->uc_stack.ss_size = 16384;
		main_thread->status = ACTIVE;
		enqueue(main_thread);
		//printf("Created main thread.\n");
	}
	thread->mynode = (Node*) malloc(sizeof(Node));
	ucontext_t* context = (ucontext_t*) malloc(sizeof(ucontext_t));
	thread->mynode->context = context;
	if(getcontext((thread->mynode->context)) != 0){
			printf("error");
			exit(1);
	}
	thread->mynode->context->uc_stack.ss_sp = (char*) malloc(sizeof(char) * 16384);
	thread->mynode->context->uc_stack.ss_size = sizeof(char)*16384;
	thread->mynode->context->uc_link = main_thread->context;
	thread->mynode->status = ACTIVE;
	//printf("numOfThreads = %d\n", numOfThreads);
	thread->mynode->tid = numOfThreads;
	thread->tid = numOfThreads++;
	makecontext((thread->mynode->context), (void(*)(void)) start_routine, 1, arg);
	enqueue(thread->mynode);
	return 0;
}	



/* Write your thread exit function here...*/
void mypthread_exit(void *retval){
    
	mypthread_t* current_th = searchThread(current_tid);
	current_th->mynode->status = DONE;
	free(current_th->mynode->context);
	if (current_th->mynode->join_bool != 0) 
			{
		mypthread_t* join_th = searchThread(current_th->mynode->join_bool);
		join_th->mynode->status = ACTIVE;
		current_th->mynode->join_bool;
	}
	mypthread_t* next_th = searchNextActiveThread(current_th->tid);
	if (current_tid == next_th->tid)
		return;
	current_tid = next_th->tid;
	setcontext(next_th->mynode->context);

}
   
/* Write your thread yield function here...*/
int mypthread_yield(void){
    
	mypthread_t* current_th = searchThread(current_tid);
	mypthread_t* next_th = searchNextActiveThread(current_th->tid);

	if (current_tid == next_th->tid)
		return 0;

	current_tid = next_th->tid;
	swapcontext(current_th->mynode->context, next_th->mynode->context);
	return 0;
}

/*  Write your thread join function here...*/
int mypthread_join(mypthread_t thread, void **retval){
	//printf("Trying to join Thread %d\n", thread.tid);
	int target_tid = thread.tid;
	mypthread_t* curr_th = searchThread(current_tid);
	//printf("Thread %d is waiting on Thread %d\n",current_tid, thread.tid);
	mypthread_t* target_th = searchThread(thread.tid);
	if (target_th->mynode->status != ACTIVE) {
		return 0;
	} else {
			curr_th->mynode->status = BLOCKED;
			target_th->mynode->join_bool = current_tid;
			current_tid = target_tid;
			swapcontext(curr_th->mynode->context, target_th->mynode->context);
	}
	//printf("Here\n");
	return 0;
}
