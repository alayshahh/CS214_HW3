#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* concat(char* word1, char* word2){
    int length1 = strlen(word1);
    int length2= strlen(word2);

    char* newWord = (char *)malloc((length1 + length2 + 1) * sizeof(char));
    memcpy(newWord, word1, length1);
    memcpy(newWord + length1, word2, length2 + 1);

    return newWord;
}