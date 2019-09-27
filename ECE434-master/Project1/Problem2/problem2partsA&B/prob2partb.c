#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h> 

int length=0;
int size=100001;

int min(int* arr){
    int min=arr[0];
    for(int i=1; i<arr[size-1]; i++){
        if(arr[i]<min){
        min=arr[i];
        }
    }
    return min;
}

int max(int* arr){
int max=arr[0];
    for(int i=1; i<arr[size-1]; i++){
        if(arr[i]>max){
        max=arr[i];
        }
    }
return max;
}

int sum(int* arr){
    int s=0;
    for(int i=0; i<arr[size-1];i++){
        s=arr[i]+s;
    }
    return s;
}


int main(int argc, char** argv) {
    

FILE *inputf;
inputf=fopen("problem2systems10.txt","r");
FILE *outputf= fopen("outputf.txt","w");

if(inputf==NULL)
{
	printf("Error opening file");
	exit(-1);
}


int* arr;
char temp;
int r;
int segsize=size*4;

int segment_id=shmget(IPC_PRIVATE,segsize,IPC_CREAT|IPC_EXCL|0775);
arr=(int*)shmat(segment_id,0,0);

pid_t a;
pid_t b=getpid();
 do{
     
    do{
        r=fscanf(inputf,"%d%c", &arr[length], &temp); 
        length++; 
        } while(temp!='\n');
        
       
 }while(r!=EOF&&(a=fork())==0);


 fprintf(outputf, "Hi I'm process %d and my parent is %d\n",getpid(), getppid());
 if(r!=EOF){
     wait(NULL);
 }
 else{
    length=length-1; 
    for(int i=0;i<length;i++){ 
        printf("%d\n",arr[i]); 
    } 
 }
        
if(a==0){
arr[size-1]=length;
}
        
else if(getpid()==b){
fprintf(outputf,"Max=%d\n",max(arr));
fprintf(outputf, "Min=%d\n",min(arr));
fprintf(outputf,"Sum=%d\n",sum(arr));
}


fclose(outputf);
fclose(inputf);

shmdt((void*)arr);
shmctl(segment_id,IPC_RMID,NULL);
exit(0);
    return (EXIT_SUCCESS);
}




