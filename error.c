#include <stdlib.h>
#include <stdio.h>

void error(char* message){
	fprintf(stderr, "Error: %s\n", message);
	exit(EXIT_FAILURE);
}
