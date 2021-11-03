#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
void f(){
	fork();
	printf("hello from F\n");
}
int main (int argc, char** argv){
	fork();
	f();
	fork();
	printf("hello from main\n");
}
