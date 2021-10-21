
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main (int argc, char **argv){
	
	char* input = NULL;
	size_t n = 0;

	printf("> ");
	while(getline(&input, &n, stdin) > 0 ){
		printf("%s", input);
		printf("> ");

//		Uncomment to test for clean input
//		if(strcmp(input, "exit") == 0){
//			break;
//		}

		input = NULL;
	}

	return EXIT_SUCCESS;
}
