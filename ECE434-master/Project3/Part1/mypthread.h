#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <ucontext.h>

//define file

#ifndef MYPTHREAD_H
#define MYPTHREAD_H

struct itimerval timer1;
struct itimerval timer2;
struct itimerval timer3;

typedef unsigned int mypthread_t;

typedef enum thread_completion_status{
  	READY,
  	RUNNING,
  	KILLED,
  	YIELD
} thread_status_state;

typedef struct threadBlock
{
  	int thread_priority;             //this is how the scheduler determines which thread to run next
  	mypthread_t thread_id;
  	void *return_value;
  	thread_status_state thread_status;
	  ucontext_t thread_context;
  	struct threadBlock *next;        //pointer to next thread to be run
} threadblock;

typedef struct Node
{
  	threadblock *thread_block;
  	struct qNode *next;
}   Node;

typedef struct schedulerQueue
{
  	int size;
  	Node *head;
  	Node *tail;
} waitLine;


typedef struct scheduler          // scheduler data structure that controls the three running queues
{
  	waitLine *run_wait_line1; 
  	waitLine *run_wait_line2; 
  	waitLine *run_wait_line3; 
  	waitLine *exit_queue;
	  threadblock *current_thread; 
  	threadblock *main_thread;    
  	int current_wait_number;
} Scheduler;


// prototypes

Node *createNode(threadblock *block); 
waitLine *createLine();
void enqueue(waitLine *x, threadblock *block); 
Node *dequeue(waitLine *x); 
Node *exitCheck(mypthread_t thread);

int mypthread_create(mypthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg);
int mypthread_yield();
void mypthread_exit(void *val_ptr);
int mypthread_join(mypthread_t thread, void **val_ptr);
void cust_timer_init();
void cust_sig_handler(int signum);
void cust_scheduler_init(); 
void cust_scheduler_handler(); 
void cust_scheduler_add(threadblock *block); 


/* Don't touch anything after this line.
 *
 * This is included just to make the mtsort.c program compatible
 * with both your ULT implementation as well as the system pthreads
 * implementation. The key idea is that mutexes are essentially
 * useless in a cooperative implementation, but are necessary in
 * a preemptive implementation.
 */


typedef int mypthread_mutex_t;
typedef int mypthread_mutexattr_t;

static inline int mypthread_mutex_init(mypthread_mutex_t *mutex,
          const mypthread_mutexattr_t *attr) { return 0; }

static inline int mypthread_mutex_destroy(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_lock(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_trylock(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_unlock(mypthread_mutex_t *mutex) { return 0; }

#endif

//end defining file
