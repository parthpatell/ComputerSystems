//Synchronization Strategy 1: Semaphores

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

int MAX = 1000000;

sem_t tSem;
sem_t cSem;

float ratio;
float currRatio;

sem_t rMutex;
sem_t tMutex;

//functions for teacher process

void teacher_enter() {
	
	sem_post(&tSem);
	printf("Teacher has entered. TID: %u\n", (int)pthread_self());
	
}

void teach() {
	
	printf("Simulating teach function for teacher %u.\n", (int)pthread_self());
	sleep(5);
}

void teacher_exit() {
	
	int teacherCount;
	int childCount;
	
	sem_getvalue(&tSem, &teacherCount);
	sem_getvalue(&cSem, &childCount);
	
	float tempRatio;
	
	if((teacherCount-1)!=0) {
		tempRatio = childCount/(teacherCount-1);
	}else if(childCount==0) {
		tempRatio=0;
	}else{
		tempRatio=MAX;
	}
	
	if(tempRatio<=ratio) {
		
		sem_wait(&tSem);
		printf("Teacher has exited. TID: %u\n", (int)pthread_self());
	
	} else {
		
		printf("Teacher attempted to leave, but this is not possible. Returning to office. TID: %u\n", (int)pthread_self());
		
		teach();
		teacher_exit();
		
	}
	
}

//functions for child process

void child_enter() {
	
	sem_post(&cSem);
	printf("Child has entered. TID: %u\n", (int)pthread_self());
	
}

void child_exit() {
	
	sem_wait(&cSem);
	printf("Child has left. TID: %u\n", (int)pthread_self());
	
}

void learn() {
	
	printf("Simulating learn function for child %u.\n", (int)pthread_self());
	sleep(5);
	
}

//functions for parent process

void parent_enter() {
	
	printf("A parent has entered. TID: %u\n", (int)pthread_self());
	
}

void parent_exit() {
	
	printf("Parent has left. TID: %u\n", (int)pthread_self());
	
}

void verify_compliance() {
	
	int teacherCount;
	int childCount;
	
	sem_getvalue(&tSem, &teacherCount);
	sem_getvalue(&cSem, &childCount);
	
	if(teacherCount!=0) {
		currRatio = childCount/teacherCount;
	} else if(childCount==0) {
		currRatio = 0;
	} else {
		currRatio = MAX;
	}
	
	sem_post(&rMutex);
	
	if(currRatio <= ratio) {
		printf("Compliance has been verified by parent %u\n", (int)pthread_self());
	} else {
		printf("Parent %u reports a violation of compliance!\n", (int)pthread_self());
	}
	
}

void go_home(){
	printf("The person with TID %u has gone home.\n", (int)pthread_self());
	pthread_exit(0);
}
	
void Teacher() {
	
	for(;;) {
		teacher_enter();
		//critical section
		teach();
		sem_wait(&tMutex);
		teacher_exit();
		sem_post(&tMutex);
		go_home();
	}
	
}

void Child() {
	
	for(;;) {
		child_enter();
		//critical section
		learn();
		child_exit();
		go_home();
	}
	
}

void Parent() {
	
	for(;;) {
		parent_enter();
		//critical section
		verify_compliance();
		parent_exit();
		go_home();
	}
	
}

int main(int argc, char** argv) {
	
	sem_init(&cSem, 0, 0);
	sem_init(&tSem, 0, 0);
	sem_init(&rMutex, 0, 1);
	sem_init(&tMutex, 0, 1);
	
	ratio = 3.0;
	
	//variables
	
	int teacherTotal = 4;
	int childTotal = 8;
	int parenttotal = 3;
	
	//initialize threads
	
	pthread_t teacherTID[teacherTotal];
	pthread_t childTID[childTotal];
	pthread_t parentTID[parentTotal];
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	
	int i;
	
	for(i=0; i<teacherTotal; i++) {
		
		pthread_create(&teacherTID[i], &attr, (void*) Teacher, NULL);
		
	}
	
	for(i=0; i<childTotal; i++) {
		
		pthread_create(&childTID[i], &attr, (void*) Child, NULL);
		
	}
	
	for(i=0; i<parentTotal; i++) {
		
		pthread_create(&parentTID[i], &attr, (void*) Parent, NULL);
		sleep(3);
		
	}
	
	for(i=0; i<teacherTotal; i++) {
		
		pthread_join(&teacherTID[i], NULL);
		
	}
	
	for(i=0; i<childTotal; i++) {
		
		pthread_join(&childTID[i], NULL);
		
	}
	
	for(i=0; i<parentTotal; i++) {
		
		pthread_join(&parentTID[i], NULL);
		
	}
	
	sem_destroy(&cSem);
	sem_destroy(&tSem);
	sem_destroy(&rMutex);
	sem_destroy(&tMutex);
	
	return 0;
	
}