#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_CHILDREN 10
#define SIZE 0

struct tree_node
{
    char* name;
    int children_no;
    struct tree_node* children[MAX_CHILDREN];
    pid_t pid;
};



// prototypes
struct tree_node* read_tree_file(const char* filename);
void print_tree(struct tree_node* root);
void __print_tree(struct tree_node* root, int level);
void process_tree(struct tree_node* root);
void __process_tree(struct tree_node* root, int level);
void __continue_tree(struct tree_node* root);
void wait_for_children();



int main(int argc, const char *argv[])
{
    struct tree_node* root = read_tree_file("tree.txt");
    // print_tree(root);

    process_tree(root);
    return 0;
}

struct tree_node* read_tree_file(const char* filename)
{
    FILE *fp;
    fp = fopen(filename, "r");

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    char* token;
    char* delimiter = " \n";

    int num_nodes = 0;

    while((read = getline(&line, &len, fp)) != -1)
    {
	num_nodes++;
    }
    rewind(fp);

    // create array of nodes, without root node
    struct tree_node* nodes[num_nodes];

    // instantiate struct and fill node name and children_no
    for(int i=0;i<num_nodes;i++)
    {
	nodes[i] = (struct tree_node*)malloc(sizeof(struct tree_node));
	getline(&line, &len, fp);

	nodes[i]->name = (char*)malloc(sizeof(char));

	strcpy(nodes[i]->name, strtok(line,delimiter));
	nodes[i]->children_no = atoi(strtok(NULL, delimiter));

	for(int j=0;j<nodes[i]->children_no;j++)
	{
	    token = strtok(NULL, delimiter);
	}
    }

    rewind(fp);
    // iterate again to fill children node pointers
    for(int i=0;i<num_nodes;i++)
    {
	getline(&line, &len, fp);
	strtok(line, delimiter);
	strtok(NULL, delimiter);

	// iterate through children
	for(int j=0;j<nodes[i]->children_no;j++)
	{
	    token = strtok(NULL, delimiter);
	    for(int k=0;k<num_nodes;k++)
	    {
		if(strcmp(token,nodes[k]->name)==0)
		{
		    nodes[i]->children[j]=nodes[k];
		}
	    }
	}
    }

    return nodes[0];
}

void print_tree(struct tree_node* root)
{
    __print_tree(root, 0);
}

void __print_tree(struct tree_node* root, int level)
{
    for(int i=0;i<level;i++)
    {
	printf("\t");
    }
    printf("%s\n", root->name);

    for(int i=0; i<root->children_no; i++)
    {
	__print_tree(root->children[i], level+1);
    }
}



void process_tree(struct tree_node* root)
{
    printf("ROOT\n");
    printf("pid[%d]\n", getpid());
    printf("node[%s], numchildren[%d]\n", root->name, root->children_no);
    printf("~~~~~~~~~~~~~~~~~\n");
    root->pid = getpid();


    __process_tree(root, 0);
    __continue_tree(root);
}

void __process_tree(struct tree_node* root, int level)
{
    if(root->children_no ==0)
    {
	sleep(1);
	return;
    }

    int fd[2];
    pipe(fd);
    // int root_pid = getpid();

    struct tree_node* node = (struct tree_node*)malloc(sizeof(struct tree_node));

    for(int i=0; i<root->children_no; i++)
    {

	int pid = fork();

	if(pid==0)
	{

      node->pid = getpid();
	    // printf("pid[%d], parent[%d]\n", getpid()-root_pid, getppid()-root_pid);
	    printf("pid[%d], parent[%d]\n", getpid(), getppid());

	    read(fd[0], node, sizeof(struct tree_node));
	    printf("node[%s], numchildren[%d]\n", node->name, node->children_no);
	    printf("~~~~~~~~~~~~~~~~~\n");

	    __process_tree(node, 0);

      raise(SIGSTOP);
	    break;

	}
	else if(pid>0)
	{

	    write(fd[1], root->children[i], sizeof(struct tree_node));
      wait_for_children();
	    //waitpid(-1, 0, WUNTRACED);


	}
	else
	{
	    printf("error\n");
	}
	// __process_tree(root->children[i], level+1);

    }
}

void __continue_tree(struct tree_node* root){
    pid_t pid;
    if (root->children_no >0){
      for (int i = 0; i < root->children_no; i++){
        pid = root->children[i]->pid;
        kill(pid, SIGCONT);
        __continue_tree(root->children[i]);



    }
}

}

void wait_for_children(){
  waitpid(-1, 0, WUNTRACED);
}
