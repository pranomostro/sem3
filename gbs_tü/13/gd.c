#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	srandom(getpid()*time(NULL));
	fork();
	fork();
	fork();
	fork();
	fork();
	fork();
	sleep(2+rand()%7);
	printf("bla\n");
	return EXIT_SUCCESS;
}
