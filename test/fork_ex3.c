#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


char* echoArgv1[] = { "/bin/ls", "-l","-R", NULL };
char* echoArgv[] = { "/bin/echo", "hello", NULL };

void f() {
    fork();
    execv(echoArgv1[0], echoArgv1);
}

int main() {
    fork();
    f();
    fork();
    execv(echoArgv[0], echoArgv);
}
