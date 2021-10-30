#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "childprocess.h"
#include "input.h"
#include "constants.h"
#include "internal.h"

//void handleSigint();
//void handleSigtstp();
// used to keep track of pid of foreground process, set to null when foreground process is terminated or is moved to background or is suspended


int main(int argc, char **argv)
{

	Jobs jobs;
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
		//Returns TRUE: 1 or FALSE: 0
		int isBackground = runInBackground(input);

		char **args = splitString(input, isBackground); /* block signals */

		int isIC = isInternalCommand(args[0]);

		if(isIC) {
			printf(" is an internal command\n");
		}

		/*
			input checking  
		*/

		// if it is not a built in command
		if(!isIC){
			pid_t pid = fork();
			if (pid == 0)
			{
				// get process ID for child process
				pid_t processID = getpid();
				// set group id to process ID
				setpgid(processID, processID);
				Child *child = (Child *)malloc(sizeof(Child)); /* block signals  */
				populateChild(child, args, processID, processID, isBackground, input);
				addJob(&jobs, child);
				executeChild(child);
			}
			else
			{
				if (!isBackground)
				{
					int status;
					waitpid(pid, &status, 0);
				}
			}
		}

		printf("> ");
	}

	return EXIT_SUCCESS;
}
