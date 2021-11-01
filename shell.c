#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "childprocess.h"
#include "constants.h"
#include "input.h"
#include "internal.h"

volatile Jobs jobs;

void sigintHandler() {
    sigset_t maskAll, prevAll;
    sigfillset(&maskAll);
    sigprocmask(SIG_BLOCK, &maskAll, &prevAll);

    int res = sendSignalToForeground(&jobs, SIGTERM);

    sigprocmask(SIG_SETMASK, &prevAll, NULL);
}
void sigtstpHandler() {
}
void sigchldHandler() {
	sigset_t maskAll, prevAll;
    sigfillset(&maskAll);

	pid_t pid;
	if((pid = waitpid(-1, NULL, 0)) > 0){
		sigprocmask(SIG_BLOCK, &maskAll, &prevAll);
		removeCompletedJob(&jobs, pid);
    	sigprocmask(SIG_SETMASK, &prevAll, NULL);
	}
}

int main(int argc, char **argv) {
    jobs.head = NULL;

    char *input = NULL;
    size_t n = 0;
    /* placeholder for signal handlers */
    signal(SIGINT, sigintHandler);
    signal(SIGTSTP, sigtstpHandler);
    signal(SIGCHLD, sigchldHandler);

    /* set envirnment variable so we can run commands from /bin and /usr/bin */
    setenv(PATH, PATH_VAR, 1);
    printf("> ");
    while (getline(&input, &n, stdin) > 0) {
        if(strcmp(input, "\n") == 0){
            printf("> ");
            input = NULL;
            n = 0;
            continue;
        }
        //Returns TRUE: 1 or FALSE: 0
        int isBackground = runInBackground(input);

        char **args = splitString(input, isBackground); /* block signals */
    
        int isIC = isInternalCommand(args, jobs);

        sigset_t maskAll, maskOne, prevOne;
        sigemptyset(&maskOne);
        sigaddset(&maskOne, SIGCHLD);
        // if it is not a built in command
        if (!isIC) {
            sigprocmask(SIG_BLOCK, &maskOne, &prevOne); //block
            pid_t pid;
            if ((pid = fork()) == 0) {
                sigprocmask(SIG_SETMASK, &prevOne, NULL); //unblock
                executeChild(args[0], args);
            } else {
                sigprocmask(SIG_BLOCK, &maskAll, NULL); //block
                pid_t processID = pid;
                setpgid(processID, processID);
                Child *child = (Child *)malloc(sizeof(Child));
                populateChild(child, args, processID, processID, isBackground, input);
                addJob(&jobs, child);
                sigprocmask(SIG_SETMASK, &prevOne, NULL); //unblock

                if (!isBackground) {
                    int status;
                    waitpid(pid, &status, 0);
                }
            }
        }

        printf("> ");

        input = NULL;
        n = 0;
    }

    return EXIT_SUCCESS;
}
