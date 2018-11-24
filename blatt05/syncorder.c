#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int highest=0;

static const int sz=64;

pthread_cond_t ict=PTHREAD_COND_INITIALIZER;
pthread_mutex_t so=PTHREAD_MUTEX_INITIALIZER;

enum Mode { LINE, WFILE, ORDER };

struct Argument
{
	int n;
	int k;
	int m;
};

typedef struct Argument Argument;

int write_buffer(long thread, char* buffer, int len, int i, Argument* a)
{
	char* outbuf=(char*)malloc(sizeof(char)*sz);

	if(a->m==ORDER)
	{
		pthread_mutex_lock(&so);
		while(thread!=highest)
			pthread_cond_wait(&ict, &so);
	}

	sprintf(outbuf, "[%02d] %03d\t", thread, i);
	write(1, outbuf, strlen(outbuf));
	write(1, buffer, len);
	write(1, "\n", 1);
	i++;

	if(a->m==ORDER)
	{
		pthread_mutex_unlock(&so);
		if(len!=sz)
		{
			pthread_mutex_lock(&so);
			highest++;
			pthread_cond_broadcast(&ict);
			pthread_mutex_unlock(&so);
		}
	}

	return 0;
}

void* dump(void *t)
{
	int i, n, fd;
	ssize_t st;
	char* filename=(char*)malloc(1+strlen(".txt")+1), * inbuf=(char*)malloc(sizeof(char)*sz);
	Argument *a=(Argument*)t;

	i=0;
	n=a->n++;

	sprintf(filename, "%d.txt", n);

	fd=open(filename, O_RDONLY);

	if(a->m==WFILE)
		pthread_mutex_lock(&so);
	while((st=read(fd, inbuf, sz))>0)
	{
		if(a->m==LINE)
			pthread_mutex_lock(&so);
		write_buffer(n, inbuf, st, i, a);
		if(a->m==LINE)
			pthread_mutex_unlock(&so);
		i++;
	}
	if(a->m==WFILE)
		pthread_mutex_unlock(&so);

	return NULL;
}

int main(int argc, char** argv)
{
	char c;
	int status;
	pthread_t *threads;
	Argument *a=(Argument*)malloc(sizeof(Argument));

	a->k=2;
	a->m=LINE;

	while((c=getopt(argc, argv, "n:flo"))>-1)
		switch(c)
		{
		case 'n':
			a->k=atoi(optarg);
			if(a->k>10)
			{
				perror("too many threads required, exiting");
				exit(1);
			}
		case 'l':
			a->m=LINE;
			break;
		case 'f':
			a->m=WFILE;
			break;
		case 'o':
			a->m=ORDER;
			break;
		default:
			perror("unknown flag");
			exit(1);
			break;
		}

	a->n=0;
	threads=(pthread_t*)malloc(a->k*sizeof(pthread_t));

	for(int i=0; i<a->k; i++)
	{
		status=pthread_create(&threads[i], NULL, dump, a);
		if(status)
		{
			perror("could not create thread");
			exit(1);
		}
	}

	for(int i=0; i<a->k; i++)
		pthread_join(threads[i], NULL);

	return 0;
}
