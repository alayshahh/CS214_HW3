#include <stdlib.h>
#include <stdio.h>

int main() {
    char* path = getenv("PATH");
    printf("%s\n", path);
}