#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
pid_t Fork(void){
	pid_t pid;
	if ((pid = fork()) < 0)
		perror("Fork error");
	return pid;
}
int main() {
	pid_t pid;
	int x = 1;
	pid = Fork(); 
	if (pid == 0) { 
		 /* Child */
		x = x+1;
		printf("child : x=%d\n", x); 
		exit(0);
	}
	/* Parent */
	x = x - 1;
	printf("parent: x=%d\n", x); 
	exit(0);
}
