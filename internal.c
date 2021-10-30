#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "constants.h"

int isInternalCommand(char* input){

	printf("%s", input);

	if(strcmp("fg", input) == 0){ //fg <JobID>
		return TRUE;
	} else 
	if(strcmp("bg", input) == 0){ //bg <JobID>
		return TRUE;
	} else 
	if(strcmp("exit", input) == 0){
		return TRUE;
	} else 
	if(strcmp("kill", input) == 0){ //kill <JobID>
		return TRUE;
	} else 
	if(strcmp("jobs", input) == 0){
		return TRUE;
	} else 
	if(strcmp("cd", input) == 0){ //cd [PATH]
		return TRUE;
	}


	return FALSE;
}