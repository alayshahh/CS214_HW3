#include <stdlib.h>
#define TRUE 1
#define FALSE 0

char *const ENVP [] = {"PATH=/bin:/usr/bin", NULL};
char DELIMITER [3] = " \n";
char BACKGROUND_PATTERN [8]= ".*&\\s*$";