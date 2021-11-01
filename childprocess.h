#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct Child {
    //used to identify job by shell user
    int jobID;
    // process ID of child process
    pid_t processID; 
    //group ID for process
    pid_t groupID;
    // boolean [0,1]
    int isBackground;
    // boolean: 1 u
    int isSuspended;
    //the program that is actually running
    char *command;
    // the args for the program that is running
    char **argv;
    //need to free the input from the user once we are done
    char* input;
    struct Child* next;
} Child;

//wrapper class that stores a linked list of all child processes
typedef volatile struct Jobs{
    Child* head;
} Jobs;

void printAllJobs(Jobs* jobs);
void addJob(Jobs* jobs, Child* newJob);
int removeCompletedJob(volatile Jobs *jobs, int pid);
int sendSignalToJob(volatile Jobs *jobs, int jobID);
int sendSignalToForeground(volatile Jobs *jobs, int signal);
void populateChild(Child *child, char** argv, pid_t processID, pid_t groupID, int isBackground, char* input);
int executeChild(char* command, char** args);
Child* getJobByID(char* jobIDstr, Jobs jobs);


