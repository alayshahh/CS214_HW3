#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "childprocess.h"
#include "constants.h"

void printAllJobs(Jobs *jobs)
{
    Child *head = jobs->head;
    while (head != NULL)
    {
        printf("[%d] %d ", head->jobID, head->processID);
        if (head->isSuspended)
        {
            printf("Stopped ");
        }
        else
            printf("Running ");
        printf("%s", head->command);
        if (head->isBackground)
            printf(" &");
        printf("\n");
        head = head->next;
    }
}

void addJob(Jobs *jobs, Child *newJob)
{
    /*
        Sets jobID for the new process and adds it to the list of jobs
    */

    if (jobs->head == NULL)
    { /* if the job list is empty then the jobID of 1 is given*/
        newJob->jobID = 1;
    }
    else
    {
        newJob->jobID = jobs->head->jobID + 1; /* other wise just add 1 to the newest job */
    }
    newJob->next = jobs->head;
    jobs->head = newJob;
}

int removeCompletedJobs(Jobs *jobs)
{
    /*
        After a SIGCHLD signal is receieved, this function is called and it will reap all termianted children 
    */

    Child *ptr = jobs->head;
    Child *prev = NULL;
    while (ptr != NULL)
    {
        int status;
        int w = waitpid(ptr->processID, &status, WNOHANG);
        if (w == -1)
        {
            perror("waitpid error");
            return FALSE;
        }
        if (WIFEXITED(status) || WIFSIGNALED(status))
        {
            // block signals

            if (prev == NULL)
            {
                jobs->head = ptr->next;
                free(ptr->argv);
                free(ptr->input);
                free(ptr);
                ptr = jobs->head;
            }
            else
            {
                prev->next = ptr->next;
                free(ptr->command);
                free(ptr);
                ptr = prev->next;
            }
        }
        else
        {
            prev = ptr;
            ptr = ptr->next;
        }
    }
    return EXIT_SUCCESS;
}

int sendSignalToJob(Jobs *jobs, int jobID, int signal)
{
    /*
        Sends signal to a child with certain jobID
        Returns TRUE(1) iif everything works otherwise returns false if there is an error or the jobID doesnt exist
    */
    Child *ptr = jobs->head;
    while (ptr != NULL)
    {
        if (ptr->jobID == jobID)
        {
            int s = kill(ptr->processID, signal);
            if (s == -1)
            {
                //error sednign signal
                perror("kill");
                return FALSE;
            }
            else
                return TRUE; //all is good
        }
    }
    //job id doesnt exist
    return FALSE;
}

void populateChild(Child *child, char **input, pid_t processID, pid_t groupID, int isBackground, char* input)
{
    child->command = input[0];
    child->argv = input;
    child->processID = processID;
    child->groupID = groupID;
    child->isBackground = isBackground;
    child->next = NULL;
    child->isSuspended = FALSE;
    child->input = input;
}

int executeChild(Child *child)
{
    if (execvp(child->command, child->argv) == -1)
    {
        perror();
        printf("%s: command not found", child->command);
        return FALSE;
    }
    return TRUE;
}
