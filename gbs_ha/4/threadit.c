#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct Argument
{
	int k;
	int r;
};

typedef struct Argument Argument;

void* countup(void *t)
{
	int k, r;
	Argument *a=(Argument*)t;
	srandom(time(NULL)+((unsigned int)pthread_self()));

	k=a->k;
	r=a->r;

	if(r)
		k=(int)random()%((int)(1.5*k+1)-k/2)+k/2;

	for(int i=1; i<=k; i++)
	{
		printf("%10u\t%d\t%ld\n", (unsigned int) pthread_self(), getpid(), i);
		sleep(1);
	}
	return NULL;
}

int main(int argc, char** argv)
{
	char c;
	int status, nflag=1;
	pthread_t *threads;
	Argument *a=(Argument*)malloc(sizeof(Argument));

	a->k=10;
	a->r=0;

	while((c=getopt(argc, argv, "k:n:r"))>-1)
		switch(c)
		{
		case 'k':
			a->k=atoi(optarg);
			break;
		case 'n':
			nflag=atoi(optarg);
			break;
		case 'r':
			a->r=1;
			break;
		default:
			perror("unknown flag");
			exit(1);
			break;
		}

	threads=(pthread_t*)malloc(nflag*sizeof(pthread_t));

	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo=localtime(&rawtime);

	printf("Start: %s", asctime(timeinfo));

	for(int i=0; i<nflag; i++)
	{
		status=pthread_create(&threads[i], NULL, countup, a);
		if(status)
		{
			perror("could not create thread");
			exit(1);
		}
	}

	for(int i=0; i<nflag; i++)
		pthread_join(threads[i], NULL);

	time(&rawtime);
	timeinfo=localtime(&rawtime);

	printf("Ende: %s", asctime(timeinfo));

	return 0;
}
