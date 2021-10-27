
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "childprocess.h"

void handleSigint();
void handleSigtstp();
// used to keep track of pid of foreground process, set to null when foreground process is terminated or is moved to background or is suspended
pid_t foregroundPID = NULL;




int main (int argc, char **argv){
	
	Jobs jobs;
	jobs.head = NULL;

	char* input = NULL;
	size_t n = 0;
	signal(SIGINT, handleSigint);
	signal(SIGTSTP, handleSigtstp);
	printf("> ");
	while(getline(&input, &n, stdin) > 0 ){
		printf("%s", input);
		printf("> ");
		/*
			input checking 
			set isBackgorund
			set char * args [] with the input 
		*/
	/*
	// if it is not a built in command
	pid_t pid = fork()
	if (pid == 0){
		//need to block signals for this malloc call (not signal safe)
		Child * child = (Child*)malloc(sizeof(Child));
		// get process ID for child process
		pid_t processID  = getpid();
		// set group id to process ID
		setpgid(processID, processID);
		populateChild(child, args, processID, processID, isBackground);
		addJob(jobs, child);
		executeChild(child);
	} else {
		if (!isBackground){
			int status;
			wait(&status);
		}
	}
*/






		input = NULL;
	}

	return EXIT_SUCCESS;
}
