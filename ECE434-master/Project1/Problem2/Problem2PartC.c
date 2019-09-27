#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#define SHSIZE 100

void merge(int array[], int left, int middle, int right);
void mergeSort(int array[], int left, int right);
char* itoa(int val, int base);

int main(int argc, char** argv){
	int numCount = 0;
	int currentNum=0, i;
	char buffer[100];
	int min, max;

	FILE* fp = fopen("problem2systems.txt","r");

	if (fp == NULL){
		printf("File problem2systems.txt is missing.\n");
		exit(0);
	}

	//Count how many numbers there are
	while(!feof(fp)){
		fgets(buffer,99,fp);
		if (strlen(buffer) == 0){
			memset(buffer,'\0',100);
			continue;
		} else {
		memset(buffer,'\0',100);
		numCount++;
		currentNum++;
	}
	}
	fclose(fp);

	//Create and load Array
	int array[numCount];
	fp = fopen("problem2systems.txt","r");
	for (i=0; i<numCount; i++){
		fgets(buffer,99,fp);
		if (strlen(buffer) == 0){
			memset(buffer,'\0',100);
			continue;
		}
		array[i] = atoi(buffer);
		memset(buffer,'\0',100);
	}
	fclose(fp);
	mergeSort(array, 0, numCount-1);

	int shmid = shmget(IPC_PRIVATE,SHSIZE, IPC_CREAT |IPC_EXCL| 0775);
	int* transfer = (int*)shmat(shmid,0,0);

	pid_t pid= fork();
	if (pid == 0){
		printf("Hi I'm process %d and my parent is %d\n",getpid(), getppid());
		int firstHalfSum = 0;
		int i;

		int shmid2 = shmget(IPC_PRIVATE,SHSIZE, IPC_CREAT |IPC_EXCL| 0775);
		int* transfer2 = (int*)shmat(shmid2,0,0);
		pid_t pid2= fork();
		if (pid2 == 0){
			printf("Hi I'm process %d and my parent is %d\n",getpid(), getppid());
			for(i=0; i<numCount/4;i++){
				firstHalfSum += array[i];
				transfer2[0] = firstHalfSum;
			}
		} else {
			printf("Hi I'm process %d and my parent is %d\n",getpid(), getppid());
			for(i=numCount/4; i<numCount/2;i++){
				firstHalfSum += array[i];
			}
		}
		wait(&pid2);
		transfer[0] = firstHalfSum+transfer2[0];

	} else { //parent process
		printf("Hi I'm process %d and my parent is %d\n",getpid(), getppid());
		int secondHalfSum = 0;
		int i;
		for (i=(numCount/2);i<numCount;i++){
			secondHalfSum += array[i];
		}
		int shmid3 = shmget(IPC_PRIVATE,SHSIZE, IPC_CREAT |IPC_EXCL| 0775);
		int* transfer3 = (int*)shmat(shmid3,0,0);
		pid_t pid3 = fork();
		if(pid3 == 0){
			printf("Hi I'm process %d and my parent is %d\n",getpid(), getppid());
			max = array[numCount-1];
			transfer3[0] = max;
			exit(0);
		} else {
			printf("Hi I'm process %d and my parent is %d\n",getpid(), getppid());
			wait(&pid3);
			min = array[0];
			printf("Max=%d\n", transfer3[0]);
			printf("Min=%d\n", min);
		}
		wait(&pid);
		int totalSum = transfer[0] + secondHalfSum;
		printf("Sum=%d\n",totalSum);

	}

	return 0;
}

void merge(int array[], int left, int middle, int right){
    int i, j, k;
    int lengthA = middle - left + 1;
    int lengthB =  right - middle;
    int ListA[lengthA], ListB[lengthB];

    for (i = 0; i < lengthA; i++){
		ListA[i] = array[left + i];
	}
    for (j = 0; j < lengthB; j++){
        ListB[j] = array[middle + 1+ j];
	}
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = left; // Initial index of merged subarray
    while (i < lengthA && j < lengthB){
        if (ListA[i] <= ListB[j])
        {
            array[k] = ListA[i];
            i++;
        }
        else{
            array[k] = ListB[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
       are any */
    while (i < lengthA){
        array[k] = ListA[i];
        i++;
        k++;
    }
    /* Copy the remaining elements of R[], if there
       are any */
    while (j < lengthB){
        array[k] = ListB[j];
        j++;
        k++;
    }
}
/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSort(int array[], int left, int right){
    if (left < right){
        int middle = left+(right-left)/2;
        // Sort first and second halves
        mergeSort(array, left, middle);
        mergeSort(array, middle+1, right);
        merge(array, left, middle, right);
    }
}

char* itoa(int val, int base){

	static char buf[32] = {0};

	int i = 30;

	for(; val && i ; --i, val /= base)

		buf[i] = "0123456789abcdef"[val % base];

	return &buf[i+1];

}
