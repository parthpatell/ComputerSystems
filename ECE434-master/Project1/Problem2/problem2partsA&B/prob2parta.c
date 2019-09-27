
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int length=0;

int min(int arr[]){
    int min=arr[0];
    for(int i=1; i<length; i++){
        if(arr[i]<min){
        min=arr[i];
        }
    }
    return min;
}

int max(int arr[]){
int max=arr[0];
    for(int i=1; i<length; i++){
        if(arr[i]>max){
        max=arr[i];
        }
    }
return max;
}

int sum(int arr[]){
    int s=0;
    for(int i=0; i<length;i++){
        s=arr[i]+s;
    }
    return s;
}

int main(int argc, char** argv) {
FILE *inputf;
inputf=fopen("problem2systems100k.txt","r");
FILE *outputf= fopen("outputf.txt","w");

if(inputf==NULL)
{
	printf("Error opening file");
	exit(-1);
}
       
 int arr[100000];
 char temp;
 int r; 
 while(r!=EOF){
    do{
        r=fscanf(inputf,"%d%c", &arr[length], &temp); 
        length++; 
        } while(temp!='\n');
 }
    length=length-1; 
    for(int i=0;i<length;i++){ 
        printf("%d\n",arr[i]); 
    } 
    
fprintf(outputf, "Hi I'm process %d and my parent is %d\n",getpid(), getppid());
fprintf(outputf,"Max=%d\n",max(arr));
fprintf(outputf, "Min=%d\n",min(arr));
fprintf(outputf,"Sum=%d\n",sum(arr));
fclose(outputf);
fclose(inputf);
    return (EXIT_SUCCESS);
}

