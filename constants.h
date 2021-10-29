#include <stdlib.h>

#define TRUE 1
#define FALSE 0


static char WHITESPACE_DELIMITER [3] = " \n";
static char* AMPERSAND = "&";
static char BACKGROUND_PATTERN [9]= "^.*&\\s*$";
static char PATH [5] = "PATH";
static char PATH_VAR [14] = "/bin:/usr/bin";
