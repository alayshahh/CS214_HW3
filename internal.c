#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "constants.h"
#include "childprocess.h"

int isInternalCommand(char** input, Jobs jobs){
	
	if(strcmp("fg", input[0]) == 0){ //fg <JobID>
		return TRUE;
	} else 
	if(strcmp("bg", input[0]) == 0){ //bg <JobID>
		return TRUE;
	} else 
	if(strcmp("exit", input[0]) == 0){
		kill(getpid(), SIGHUP);
		return TRUE;
	} else 
	if(strcmp("kill", input[0]) == 0){ //kill <JobID>
		Child* child = getJobByID(input[1], jobs);
		kill(child->processID, SIGINT);
		return TRUE;
	} else 
	if(strcmp("jobs", input[0]) == 0){
		printAllJobs(&jobs);
		return TRUE;
	} else 
	if(strcmp("cd", input[0]) == 0){ //cd [PATH]
		chdir(input[1]);

		// TODO: Do we have to update PWD env var
		// char* pwd = getenv("PWD");
		// printf("%s\n", pwd);
		return TRUE;
	}

	return FALSE;
}