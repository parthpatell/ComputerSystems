#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

FILE* output;
output=fopen("Output,txt", "w");

struct procInfo
{
    char procName; 
    int numChild;
    char children[5];
    int return_Value;
};

struct procInfo proc[] = {
    { 'A', 2, {'B', 'C'}, 2},
    { 'B', 1, {'D'}, 4 },
    { 'C', 0, { }, 6 },
    { 'D', 0, { }, 10 },
};

char procId = 'A'; 
int Value;

void procfunction()
{
    int i, j;
    int n = sizeof(proc) / sizeof(struct procInfo);
    fprintf(output,"Started process: %c, that has pid:%d\n", procId, getpid());
    for(i = 0; i < n; i++)
    {
        if(procId == proc[i].procName) 
            break;
    }
    if(i < n) 
    {
        Value = proc[i].return_Value;
        pid_t pids[5];
        for(j = 0; j < proc[i].numChild; j++)
        {
            pids[j] = fork();
            if(pids[j] < 0)
            {
                fprintf(output, "Proc %c, pid=%d: fork failed\n", procId, getpid());
            }
            if(pids[j] == 0)
            {
                procId = proc[i].children[j];
                procfunction();
                return;
            }
            else
            {
                fprintf(output, "Proc %c, pid=%d: Forked %c, pid=%d\n", procId, getpid(), proc[i].children[j], pids[j]);
            }
        }
        fprintf(output, "Proc %c, pid=%d: Waiting for children to end\n", procId, getpid());
        for(j = 0; j < proc[i].numChild; j++)
        {
            int status;
            if(pids[j] > 0) 
            {
                waitpid(pids[j], &status, 0);
                fprintf(output, "Proc %c, pid=%d: Child exited with status %d\n", procId, getpid(), WEXITSTATUS(status));
            }
        }
    }
    sleep(10);
    fprintf(output,"Proc %c, pid=%d: ending proc\n", procId, getpid());
}
int main()
{
    procfunction();
    return Value;
}
