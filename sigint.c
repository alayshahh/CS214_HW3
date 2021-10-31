#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "childprocess.h"
#include "constants.h"
#include <signal.h>


pid_t foregroundProcess;

void handler(){
    printf("%d\n", foregroundProcess);
    exit(0);
}

int main(){
    signal(SIGINT, handler);

    foregroundProcess = fork();
    if(foregroundProcess == 0){
        while(1){
            ;
        }
    } else {
        int childStatus;
        printf("%d\n", foregroundProcess);
        pid_t wpid = waitpid(foregroundProcess, &childStatus, 0);
        if(WIFEXITED(childStatus)){
            printf("Child %d terminated with exit status good\n", wpid);
        } else {
            printf("child %d terminated abnormally\n", wpid);
        }
    }
}

