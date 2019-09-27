#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>

typedef struct tree_nodes{
	int children_no; //number of children
	char* name; //name of process
	struct tree_nodes *childrenList; //pointer to rest of children
} tree_node;

typedef struct Node threadList;
struct Node{
	pthread_t tid;
	struct Node* next;
	struct Node* prev;
};

struct Passer {
	tree_node *currentNode;
	int childNumber;
};

tree_node* read_tree_file(const char *filename);
void print_tree(tree_node *root);
void _print_tree(tree_node *root, int level);
void process_tree(tree_node *root, int initial);

threadList* threads=NULL;

void* threadFunction(void *threadargs) {
	tree_node* new = threadargs;
	sleep(5);
	printf("Thread created, TID %d\n", (int)pthread_self());
	process_tree(new, 0);
	return(NULL);
}

tree_node* read_tree_file(const char *filename){
	/*		Create Variables	*/
	tree_node *root; //array of nodes
	//tree_node newNode;
	int newNode_no, index = 0;
	int childrenOffset = 2;
	FILE *fp;
	char buffer[500];
	char *token;
	int numNodes = 0;

	/*		Count nodes and create array 	*/
	fp = fopen(filename, "r"); //file pointer
	if (fp == NULL){
		return 0;
	}
	while (!feof(fp)){
		numNodes++;
		fgets(buffer, 50, fp);
	}
	fclose(fp);
	root = (tree_node*) malloc(sizeof(tree_node)*numNodes);
	threads = (threadList*)malloc(sizeof(threadList)*numNodes);
	threads->next=NULL;

	/*		Read File and fill array	*/
	fp = fopen(filename, "r");
	//printf("READING...\n");
	while(fgets(buffer, 500, fp) != NULL){		//read each line
		//printf("New while iteration\n");
		childrenOffset--;		//offset between parent and child node in array

		token = strtok(buffer, " \n"); 	//get name from file
		root[index].name = (char*) malloc(sizeof(token));
		strcpy(root[index].name, token);

		token = strtok(NULL, " \n"); 		//get number of children from file
		newNode_no = atoi(token);
		//printf("%s has %d children\n",root[index].name, newNode_no);
		if (newNode_no == 0){ //set pointer to NULL if no children
			root[index].childrenList = NULL;
		} else {
			root[index].childrenList = &root[index+childrenOffset];
		}
		root[index].children_no = newNode_no;

		childrenOffset+=newNode_no;
		index+=1;
		if (feof(fp)) break;
	}
	return root;
}

void print_tree(tree_node *root){
	_print_tree(root,1);
}

void _print_tree(tree_node *root, int level){
	if (level == 0){
		return;
	}
	int i;
	int counter = 0;
	for (i=0; i<level; i++){
		printf("%s\t", root[i].name);
		counter += root[i].children_no;
	}
	printf("\n");
	_print_tree(root+level, counter);
}

void process_tree(tree_node *root, int initial){
	tree_node *currentNode = root;
	int i;

	if (currentNode->children_no == 0){
		sleep(3);
		printf("Node %s has exited (TID: %d)\n", currentNode->name, (int)pthread_self());
		pthread_exit(NULL);
	}

	pthread_t tids[currentNode->children_no];

	for (i=0; i< currentNode->children_no; i++){
		tree_node* threadargs=(tree_node*)malloc(sizeof(tree_node));
		threadargs=currentNode->childrenList+i;
		threadList *newNode = (threadList*)malloc(sizeof(threadList));
		newNode->tid=pthread_create(&tids[i], NULL, threadFunction, (void*) threadargs);
		newNode->next=NULL;
		threads->next=newNode;
		threads=threads->next;
	}

	sleep(3);

	//if(initial == 1)
	//	kill(tids[1],SIGINT);

	for (i=0; i<currentNode->children_no; i++){
		pthread_join(tids[i], NULL);
	}
	if (initial == 1){
		printf("Root Node %s has exited (TID: %d)\n", currentNode->name, (int)pthread_self());
	} else {
		printf("Node %s has exited (TID: %d)\n", currentNode->name, (int)pthread_self());
	}
	pthread_exit(NULL);
}


int main(int argc, char** argv){
	/*		Check arguments		*/
	if (argc != 2){
		printf("Incorrect number of arguments.\nPlease use the format:\n\t./Problem1 <File_Name>\n\n");
		return 0;
	}
	tree_node *root = read_tree_file(argv[1]);
	print_tree(root);
	process_tree(root,1);


	return 0;
}
