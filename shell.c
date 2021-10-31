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
}

int main(int argc, char **argv) {
    jobs.head = NULL;

    char *input = NULL;
    size_t n = 0;
    /* placeholder for signal handlers */
    signal(SIGINT, sigintHandler);
    signal(SIGTSTP, sigtstpHandler);
    signal(SIGTSTP, sigchldHandler);

    /* set envirnment variable so we can run commands from /bin and /usr/bin */
    setenv(PATH, PATH_VAR, 1);
    printf("> ");
    while (getline(&input, &n, stdin) > 0) {
        sigset_t maskAll, maskOne, prevOne, prevAll;
        sigemptyset(&maskOne);
        sigaddset(&maskOne, SIGCHLD);

        //Returns TRUE: 1 or FALSE: 0
        int isBackground = runInBackground(input);
        printf("isBackground %d\n", isBackground);

        char **args = splitString(input, isBackground); /* block signals */

        int isIC = isInternalCommand(args, jobs);

        // if it is not a built in command
        if (!isIC) {
            sigprocmask(SIG_BLOCK, &maskOne, &prevOne);
            pid_t pid;
            if ((pid = fork()) == 0) {
                sigprocmask(SIG_SETMASK, &prevOne, NULL);
                executeChild(args[0], args);
            } else {
                sigprocmask(SIG_BLOCK, &maskAll, &prevAll);
                pid_t processID = pid;
                setpgid(processID, processID);
                Child *child = (Child *)malloc(sizeof(Child));
                populateChild(child, args, processID, processID, isBackground, input);
                addJob(&jobs, child);
                sigprocmask(SIG_SETMASK, &prevAll, NULL);

                if (!isBackground) {
                    printf("waiting\n");
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
