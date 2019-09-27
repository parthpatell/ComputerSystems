#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


struct evaluate{
	int chi;   // integer result for child expression
};
const key_t key = 1234;
pid_t pid;
void* shared_memory;

int main()
{
	// Declare input variables
	int int1;
	int int2;
	int int3;

	// Read input file
	FILE *inputFile;
	inputFile = fopen("inputFile.txt", "r");
	FILE *output_file;
	output_file = fopen("output.txt", "w");
	if (inputFile == NULL){
		fprintf(output_file, "Error! File empty.\n");
		exit(EXIT_FAILURE);
	}
	else {
		fscanf(inputFile, "%d %d %d ", int1, int2, int3);
	}

	// Create shared memory
	int shmid = shmget(key, sizeof(struct evaluate), 0666 | IPC_CREAT);
	if (shmid == -1){
		exit(EXIT_FAILURE);
	}

	// Fork child
	pid = fork();
	if (pid == 0){
		shared_memory = shmat(shmid, (void*)0, 0);
		if (shared_memory == (void*)-1){
			exit(EXIT_FAILURE);
		}
		struct evaluate* shared_data = (struct evaluate*) shared_memory;
		shared_data->chi = int2 + int3;

		if (shmdt(shared_memory) == -1){
			exit(EXIT_FAILURE);
		}
		exit(EXIT_SUCCESS);
	}

	// Wait for child process to terminate
	int status_pid;
	waitpid(pid, &status_pid, 0);

	if (!WIFEXITED(status_pid)){
		exit(EXIT_FAILURE);
	}

	// Parent attach memory
	shared_memory = shmat(shmid, (void*)0, 0);
	if (shared_memory == (void*)-1) exit(EXIT_FAILURE);
	struct evaluate* shared_data = (struct evaluate*) shared_memory;

	// Calculate and output result
	int output = (int1)*(shared_data->chi);
	fprintf(output_file, "Result is %d\n", output);

	// Detach from shared memory
	if (shmdt(shared_memory) == -1){
		exit(EXIT_FAILURE);
	}
	if (shmctl(shmid, IPC_RMID, 0) == -1){
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}