#include <stdlib.h>
#ifndef INTERNAL_H
#define INTERNAL_H

int isInternalCommand(char** args, Jobs jobs, char* input);
void cleanJobs(Jobs jobs);
void jobIDError(char* jobID);
void terminateJobs(Jobs jobs);

#endif
