#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void countup(int j)
{
	int i;
	for(i=1; i<=j; i++)
	{
		printf("%d %d %d\n", getpid(), getppid(), i);
		sleep(1);
	}
	exit((getpid()+j)%100);
}

int main(int argc, char** argv) {
	int status;
	pid_t pid;

	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo=localtime(&rawtime);

	printf("Start: %s", asctime(timeinfo));

	switch((pid=fork())) {
	case 0:
		if(argc!=2)
		{
			perror("forkone <K>");
			exit(2);
		}
		countup(atoi(argv[1]));
	case -1:
		perror("could not fork");
		exit(1);
	default:
		wait(&status);
		printf("Exit-Code: %d\n", WEXITSTATUS(status));
	}

	time(&rawtime);
	timeinfo=localtime(&rawtime);

	printf("Ende: %s", asctime(timeinfo));

	return 0;
}
