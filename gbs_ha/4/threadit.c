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

	k=r?(int)random()%((int)(1.5*k+1)-k/2)+k/2:k;

	for(int i=1; i<=k; i++)
	{
		printf("%d %d %d\n", (unsigned int) pthread_self(), getpid(), i);
		sleep(1);
	}
	exit((getpid()+k)%100);
}

int main(int argc, char** argv)
{
	char c;
	int status;
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

	printf("Start: %s", asctime(timeinfo));

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

	while(pids->first!=NULL)
	{
		wait(&status);
		printf("Exit-Code: %d\n", WEXITSTATUS(status));
		list_remove(pids, pids->last);
	}

	time(&rawtime);
	timeinfo=localtime(&rawtime);

	printf("Ende: %s", asctime(timeinfo));

	return 0;
}
