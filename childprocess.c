#include "childprocess.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "constants.h"

void printAllJobs(volatile Jobs *jobs) {
    Child *head = jobs->head;
    while (head != NULL) {
        printf("[%d] %d ", head->jobID, head->processID);
        if (head->isSuspended) {
            printf("Stopped ");
        } else
            printf("Running ");
        printf("%s", head->command);
        if (head->isBackground)
            printf(" &");
        printf("\n");
        head = head->next;
    }
}

void addJob(volatile Jobs *jobs, Child *newJob) {
    /*
        Sets jobID for the new process and adds it to the list of jobs
    */

    if (jobs->head == NULL) { /* if the job list is empty then the jobID of 1 is given*/
        newJob->jobID = 1;
    } else {
        newJob->jobID = jobs->head->jobID + 1; /* other wise just add 1 to the newest job */
    }
    newJob->next = jobs->head;
    jobs->head = newJob;
}

int removeCompletedJobs(volatile Jobs *jobs) {
    /*
        After a SIGCHLD signal is receieved, this function is called and it will reap all termianted children
    */

    Child *ptr = jobs->head;
    Child *prev = NULL;
    while (ptr != NULL) {
        int status = 0;
        int w = waitpid(ptr->processID, &status, WNOHANG);
        if (w < 0 && errno != ECHILD) {
            perror("waitpid error");
            return FALSE;
        }

        if (w != 0 && (WIFEXITED(status) || WIFSIGNALED(status))) {
            // printf("w = %d removing job [%d] PID: %d %s, exited with status %d \n", w, ptr->jobID, ptr->processID, ptr->command, status);
            if (prev == NULL) {
                jobs->head = ptr->next;
                free(ptr->argv);
                free(ptr->input);
                free(ptr);
                ptr = jobs->head;
            } else {
                prev->next = ptr->next;
                free(ptr->argv);
                free(ptr->input);
                free(ptr);
                ptr = prev->next;
            }
        } else {
            prev = ptr;
            ptr = ptr->next;
        }
    }
    return EXIT_SUCCESS;
}

int removeCompletedJob(volatile Jobs *jobs, int pid) {
    /*
        After a SIGCHLD signal is receieved, this function is called and it will reap all termianted children 
    */

    Child *ptr = jobs->head;
    Child *prev = NULL;
    while (ptr != NULL) {
        if (ptr->processID == pid) {
            if (prev == NULL) {
                jobs->head = ptr->next;
                free(ptr->argv);
                free(ptr->input);
                free(ptr);
                ptr = jobs->head;
            } else {
                prev->next = ptr->next;
                free(ptr->argv);
                free(ptr->input);
                free(ptr);
                ptr = prev->next;
            }
            break;
        } else {
            prev = ptr;
            ptr = ptr->next;
        }
    }
    return EXIT_SUCCESS;
}

int sendSignalToJob(Jobs *jobs, int jobID) {
    /*
        Sends signal to a child with certain jobID
        Returns TRUE(1) iif everything works otherwise returns false if there is an error or the jobID doesnt exist
    */
    Child *ptr = jobs->head;
    while (ptr != NULL) {
        if (ptr->jobID == jobID) {
            int s = kill(ptr->processID, SIGTERM);
            if (s == -1) {
                //error sednign signal
                perror("kill");
                return FALSE;
            } else
                return TRUE;  //all is good
        }
    }
    //job id doesnt exist
    return FALSE;
}
int sendSignalToForeground(Jobs *jobs, int signal) {
    Child *ptr = jobs->head;
    while (ptr != NULL) {
        if (!ptr->isBackground) {
            int s = kill(ptr->processID, signal);
            if (s == -1) {
                //error sednign signal
                perror("error sending signal to foreground");
                return FALSE;
            } else {
                if (signal == SIGTSTP) {  //if we send a suspend signal then we move this process to background and suspend it
                    ptr->isSuspended = TRUE;
                    ptr->isBackground = TRUE;
                }
            }
            return TRUE;  //all is good
        }
    }
    //no foreground process doesnt exist
    return FALSE;
}

void populateChild(Child *child, char **argv, pid_t processID, pid_t groupID, int isBackground, char *input) {
    child->command = argv[0];
    child->argv = argv;
    child->processID = processID;
    child->groupID = groupID;
    child->isBackground = isBackground;
    child->next = NULL;
    child->isSuspended = FALSE;
    child->input = input;
}

int executeChild(char *command, char **args) {
    if (execvp(command, args) == -1) {
        printf("%s: command not found\n", command);
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

Child *getJobByID(char *jobIDstr, Jobs jobs) {
    if (jobs.head == NULL) {
        return NULL;
    }

    int targetJobID;
    sscanf(jobIDstr, "%%%d", &targetJobID);

    Child *ptr = jobs.head;

    while (ptr != NULL) {
        if (ptr->jobID == targetJobID) {
            return ptr;
        }
        ptr = ptr->next;
    }

    return NULL;
}
