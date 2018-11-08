#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#include "list.h"

void countup(int k, int r)
{
	srandom(time(NULL)+getpid());

	for(int i=1; i<=k; i++)
	{
		printf("%d %d %d\n", getpid(), getppid(), r?(int)random()%((int)(1.5*k+1)-k/2)+k/2:i);
		sleep(1);
	}
	exit(getpid());
}

int main(int argc, char** argv)
{
	char c;
	pid_t pid;
	int kflag=10, nflag=1, rflag=0;

	while((c=getopt(argc, argv, "k:n:r"))>-1)
		switch(c)
		{
		case 'k':
			kflag=atoi(optarg);
			break;
		case 'n':
			nflag=atoi(optarg);
			break;
		case 'r':
			rflag=1;
			break;
		default:
			perror("unknown flag");
			exit(1);
			break;
		}

	list_t *pids=list_init();

	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo=localtime(&rawtime);

	printf("Start: %s\n", asctime(timeinfo));

	for(int i=0; i<nflag; i++)
		switch((pid=fork()))
		{
		case 0:
			countup(kflag, rflag);
			break;
		case -1:
			perror("could not fork");
			exit(2);
			break;
		default:
			list_append(pids, pid);
			break;
	}

	time(&rawtime);
	timeinfo=localtime(&rawtime);

	while(pids->first!=NULL)
	{
		wait(NULL);
		list_remove(pids, pids->last);
	}

	printf("Ende: %s\n", asctime(timeinfo));

	return 0;
}
