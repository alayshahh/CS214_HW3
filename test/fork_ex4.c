#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    printf("a");
    fflush(stdout);
    fork();
    printf("b");
}


