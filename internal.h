#include <stdlib.h>

int isInternalCommand(char** args, Jobs jobs, char* input);
void cleanJobs(Jobs jobs);
void jobIDError(char* jobID);