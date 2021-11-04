#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "childprocess.h"
#include "constants.h"

void terminateJobs(Jobs jobs) {
    Child* ptr = jobs.head;

    while (ptr != NULL) {
        if (ptr->isSuspended == 1) {  //stopped
            killpg(ptr->groupID, SIGHUP);
            killpg(ptr->groupID, SIGCONT);
        }
        if (ptr->isSuspended == 0) {  //running
            killpg(ptr->groupID, SIGHUP);
        }
        ptr = ptr->next;
    }
}

void jobIDError(char* jobID) {
    printf("Error: jobID: '%s' does not exist.", jobID);
}

int isInternalCommand(char** args, Jobs jobs, char* input) {
    sigset_t maskAll, prev;
    sigfillset(&maskAll);

    if (strcmp("fg", args[0]) == 0) {  //fg <JobID>
        //TODO: Check for errors

        Child* child = getJobByID(args[1], jobs);

        if (child != NULL) {
            int jobID = child->jobID;
            pid_t pid = child->processID;
            child->isBackground = FALSE;
            child->isSuspended = FALSE;
            killpg(child->processID, SIGCONT);
            int status;
            waitpid(child->processID, &status, WUNTRACED);
            if (WIFSIGNALED(status) && WTERMSIG(status) != SIGHUP) {
                sigprocmask(SIG_BLOCK, &maskAll, &prev);
                printf("[%d] %d  terminated by signal %d\n", jobID, pid, WTERMSIG(status));
                sigprocmask(SIG_SETMASK, &prev, NULL);
            }
        } else {
            sigprocmask(SIG_BLOCK, &maskAll, &prev);
            jobIDError(args[1]);
            sigprocmask(SIG_SETMASK, &prev, NULL);
        }
        return TRUE;
    } else if (strcmp("bg", args[0]) == 0) {  //bg <JobID>
        Child* child = getJobByID(args[1], jobs);
        if (child != NULL) {
            child->isSuspended = FALSE;
            child->isBackground = TRUE;
            //TODO: Do we still want to run this if the process is in the bg
            killpg(child->processID, SIGCONT);
        } else {
            sigprocmask(SIG_BLOCK, &maskAll, &prev);
            jobIDError(args[1]);
            sigprocmask(SIG_SETMASK, &prev, NULL);
        }
        return TRUE;
    } else if (strcmp("exit", args[0]) == 0) {
        terminateJobs(jobs);
        free(args);
        free(input);
        exit(EXIT_SUCCESS);
        // kill(getpid(), SIGHUP);
        return TRUE;
    } else if (strcmp("kill", args[0]) == 0) {  //kill <JobID>
        Child* child = getJobByID(args[1], jobs);
        if (child != NULL) {
            if (child->isSuspended) {
                kill(child->processID, SIGCONT);
            }
            kill(child->processID, SIGTERM);
        } else {
            sigprocmask(SIG_BLOCK, &maskAll, &prev);
            jobIDError(args[1]);
            sigprocmask(SIG_SETMASK, &prev, NULL);
        }
        return TRUE;
    } else if (strcmp("jobs", args[0]) == 0) {
        sigprocmask(SIG_BLOCK, &maskAll, &prev);
        printAllJobs(&jobs);
        sigprocmask(SIG_SETMASK, &prev, NULL);
        return TRUE;
    } else if (strcmp("cd", args[0]) == 0) {  //cd [PATH]
        char cwd[256];

        // chdir(args[1]);
        // TODO: Do we have to update PWD env var

        if (chdir(args[1]) != 0)
            perror("chdir() error()");
        else {
            if (getcwd(cwd, sizeof(cwd)) == NULL)
                perror("getcwd() error");
            // else
            // printf("current working directory is: %s\n", cwd);
            setenv("PWD", cwd, 1);
            // char* pwd = getenv("PWD");
            // printf("%s\n", pwd);
        }

        return TRUE;
    }

    return FALSE;
}
