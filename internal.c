#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include "constants.h"
#include "childprocess.h"

void terminateJobs(Jobs jobs){

	Child* ptr = jobs.head;

	while(ptr != NULL){
		if(ptr->isSuspended == 1){ //stopped
			killpg(ptr->groupID, SIGHUP);
			killpg(ptr->groupID, SIGCONT);
		}
		if(ptr->isSuspended == 0){ //running
			killpg(ptr->groupID, SIGHUP);
		}
	}
		
}

int isInternalCommand(char** input, Jobs jobs){
	
	if(strcmp("fg", input[0]) == 0){ //fg <JobID>
		//TODO: Check for errors

		Child* child = getJobByID(input[1], jobs);

		if(child != NULL){
			kill(child->processID, SIGCONT);
			waitpid(child->processID, NULL, 0);
		}
		return TRUE;
	} else 
	if(strcmp("bg", input[0]) == 0){ //bg <JobID>
		Child* child = getJobByID(input[1], jobs);

		//TODO: Do we still want to run this if the process is in the bg
		kill(child->processID, SIGCONT);
		return TRUE;
	} else 
	if(strcmp("exit", input[0]) == 0){
		terminateJobs(jobs);
		kill(getpid(), SIGHUP);
		return TRUE;
	} else 
	if(strcmp("kill", input[0]) == 0){ //kill <JobID>
		Child* child = getJobByID(input[1], jobs);
		kill(child->processID,  SIGINT);
		return TRUE;
	} else 
	if(strcmp("jobs", input[0]) == 0){
		sigset_t maskAll, prevAll;
		sigfillset(&maskAll);

		sigprocmask(SIG_BLOCK, &maskAll, &prevAll);
		printAllJobs(&jobs);
		sigprocmask(SIG_SETMASK, &prevAll, NULL);
		return TRUE;
	} else 
	if(strcmp("cd", input[0]) == 0){ //cd [PATH]
		char cwd[256];

		// chdir(input[1]);
		// TODO: Do we have to update PWD env var
		

		if (chdir(input[1]) != 0)
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

