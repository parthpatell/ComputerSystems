#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
void WriteInFile(int fd, const char *buff, int len);
void CopyFile (int fd, const char *file_in);

void WriteInFile(int fd, const char *buff, int len){
	write(fd, buff, len);
}

void CopyFile (int fd, const char *file_in){
	char buff[100];
	int len = 99;
	int test;
	int fdInput = open(file_in, O_RDONLY);
	memset(buff, '\0', 100);
	while((test=read(fdInput, buff, len)) > 0){
		WriteInFile(fd, buff, len);
		memset(buff, '\0', 100);
	}
	close(fdInput);
}

int main(int argc, char** argv){
	char * outputString;
	char * input1;
	char * input2;
	int fdInput1;
	int fdInput2;
	int fdOut;

	if (argc<3 || argc>4){ //Check for 2 or 3 input arguments (+1 for executable)
		printf("Incorrect arguments.\nMust have 2 input files and an optional output file name\n");
		printf("Format:\t./myfiles <input1> <input2> [output]\n\n");
		exit(0);
	}

	if (argc == 4){ //3 Arguments output name provided
		outputString = (char*) malloc(strlen(argv[3]) * sizeof(char)+2);
		strcpy(outputString, argv[3]);
	} else { //There are only 2 arguments, defualt outputfile name
		outputString = (char*) malloc(sizeof("myfile.out"));
		strcpy(outputString, "myfile.out");
	}

	//copy 1st and 2nd input arguments
	input1 = (char*) malloc(sizeof(char) * strlen(argv[1]) + 2);
	strcpy(input1, argv[1]);
	input2 = (char*) malloc(sizeof(char) * strlen(argv[2]) + 2);
	strcpy(input2, argv[2]);

	//Open file pointers and check if they exist
	int flag = 0; //Boolean
	fdInput1 = open(input1, O_RDONLY);
	fdInput2 = open(input2, O_RDONLY);
	if (fdInput1 == -1 ){
		printf("%s is an invalid input file\n\n", input1);
		flag = 1;
	}
	if (fdInput2 == -1 ){
                printf("%s is an invalid input file\n\n", input2);
		flag = 1;
        }
	if (flag == 1){
		close(fdInput1);
		close(fdInput2);
		free(input1);
        	free(input2);
        	free(outputString);
		exit(0);
	}
	fdOut = open(outputString, O_CREAT | O_WRONLY  | O_TRUNC);

	//Copy lines from input files to output files
	CopyFile(fdOut, input1);
	CopyFile(fdOut, input2);

	free(input1);
        free(input2);
        free(outputString);
	close(fdOut);
	return 0;
}
