#include <stdlib.h>

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define TRUE 1
#define FALSE 0

static const char WHITESPACE_DELIMITER [3] = " \n";
static const char* AMPERSAND = "&";
static const char BACKGROUND_PATTERN [9]= "^.*&\\s*$";
static const char PATH [5] = "PATH";
static const char PATH_VAR [14] = "/bin:/usr/bin";

#endif