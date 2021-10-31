#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "childprocess.h"
#include "input.h"
#include "constants.h"



volatile Jobs jobs;

int main(int argc, char **argv)
{
	jobs.head = NULL;

	char *input = NULL;
	size_t n = 0;
	/* placeholder for signal handlers */
	//signal(SIGINT, handleSigint);
	//signal(SIGTSTP, handleSigtstp);
	/* set envirnment variable so we can run commands from /bin and /usr/bin */
	setenv(PATH, PATH_VAR, 1);
	printf("> ");
	while (getline(&input, &n, stdin) > 0)
	{
		// printf("%s", input);
		sigset_t maskAll, maskOne, prevOne, prevAll;
		sigemptyset(&maskOne);
		sigaddset(&maskOne, SIGCHLD);

		printf("%s\n", input);
		int isBackground = runInBackground(input);
		printf("%d\n", isBackground);

		char **args = splitString(input, isBackground); /* block signals */
		/*
			input checking  
		*/

		// if it is not a built in command
		pid_t pid = fork();
		sigprocmask(SIG_BLOCK, &maskOne, &prevOne);
		if (pid == 0)
		{
			sigprocmask(SIG_SETMASK, &prevOne, NULL);
			executeChild(args[0], args);
		}
		else
		{
			sigprocmask(SIG_BLOCK, &maskAll, &prevAll);
			pid_t processID = getpid();
			// set group id to process ID
			setpgid(processID, processID);
			Child *child = (Child *)malloc(sizeof(Child)); 
			populateChild(child, args, processID, processID, isBackground, input);
			addJob(&jobs, child);
			printAllJobs(&jobs);
			sigprocmask(SIG_SETMASK, &prevAll, NULL);

			if (!isBackground)
			{
				int status;
				printf("waiting for child to complete yooo\n");
				waitpid(pid, &status, 0);
			}
		}

		printf("> ");
	}

	return EXIT_SUCCESS;
}
