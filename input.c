#include <regex.h>
#include <stdio.h>
#include <string.h>

#include "constants.h"

int runInBackground(char *input) {
    int status;
    regex_t re;
    if (regcomp(&re, BACKGROUND_PATTERN, REG_EXTENDED) != 0) {
        return FALSE;
    }
    status = regexec(&re, input, (size_t)0, NULL, 0);
    regfree(&re);
    if (status != 0) {
        return FALSE;
    }
    return TRUE;
}

char **splitString(char *str, int isBackground) {
    /*
    splits sting into array of "words" 
    make sure that signals are blocked during this process
   */
    char **split = NULL;
    char *word = strtok(str, WHITESPACE_DELIMITER); /* split on space and newline char*/
    int words = 0;
    /* split string and append tokens to split array */
    while (word) {
        words++;
        split = realloc(split, sizeof(char *) * words);
        if (split == NULL)
            exit(-1); /* memory allocation failed */
        split[words - 1] = word;
        word = strtok(NULL, WHITESPACE_DELIMITER);
    }
    /* realloc one extra element for the last NULL */
    if (isBackground) {
        char *lastword = split[words - 1];
        if (strcmp(lastword, AMPERSAND) == 0) {
            split[words - 1] = NULL;
        } else {
            split[words - 1] = strtok(lastword, AMPERSAND);
        }
    }
    split = realloc(split, sizeof(char *) * (words + 1));
    split[words] = 0;
    return split;
}
