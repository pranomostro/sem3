#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

enum Algorithm { RR, PRR, SRTN, UNDEF };

struct Process
{
	int prio;
	int start;
	int runtime;
	int rantime;
};

typedef enum Algorithm Algorithm;
typedef struct Process Process;

void print_time_step (int time, int thread_num ) {
	static int first_time = 1;
	int i;
	if(first_time)
	{
		printf("  Time |  1  2  3  4  5  6  7  8  9  10\n");
		printf("−−−−−−−+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−\n");
		first_time=0;
	}
	printf("%06d |",time);
	if(thread_num)
	{
		for(i=1; i<thread_num; i++)
			printf("   ");
		printf("  %d\n", thread_num);
	}
	else
		printf("\n");
}

int main(int argc, char** argv)
{
	char c;
	int nflag, tflag, qflag, curthread, remaining, now, i, j;
	Algorithm aflag=UNDEF;
	Process *threads;

	while((c=getopt(argc, argv, "n:t:q:a"))>-1)
		switch(c)
		{
		case 'n':
			nflag=atoi(optarg);
			if(nflag>10)
			{
				perror("n>10, exiting");
				exit(1);
			}
			break;
		case 't':
			tflag=atoi(optarg);
			break;
		case 'q':
			qflag=atoi(optarg);
			break;
		case 'a':
			if(strcmp(optarg, "RR"))
				aflag=RR;
			else if(strcmp(optarg, "PRR"))
				aflag=PRR;
			else if(strcmp(optarg, "SRTN"))
				aflag=SRTN;
			else
			{
				perror("unknown algorithm, exiting");
				exit(1);
			}
			break;
		default:
			perror("unknown flag");
			exit(1);
			break;
		}

	if(aflag==UNDEF)
	{
		perror("no algorithm specified, exiting");
		exit(1);
	}

	threads=(Process*)malloc(nflag*sizeof(Process));

	for(i=0; i<nflag; i++)
	{
		scanf("%d %d %d", &(threads[i].prio), &(threads[i].start), &(threads[i].runtime));
		if(threads[i].start>100000||threads[i].runtime>30000||threads[i].prio<1||threads[i].prio>10)
		{
			perror("input too big, exiting");
			exit(1);
		}
		threads[i].rantime=0;
	}

	for(i=0; i<nflag; i++)
		printf("%d: prio %d, start %d, runtime %d, rantime %d\n", i, threads[i].prio, threads[i].start, threads[i].runtime, threads[i].rantime);

	curthread=-1;
	remaining=1;

	for(now=0; remaining!=0; now+=tflag)
	{
		switch(aflag)
		{
		case RR:
			if(now%qflag==0)
			{
				for(j=(curthread==-1?0:curthread); i!=j; i=(i+1)%nflag)
				{
					printf("i: %d, j: %d\n", i, j);
					if(now>=threads[i].start&&threads[i].runtime-threads[i].rantime>0)
					{
						printf("setting curthread to %d\n", i);
						curthread=i;
						threads[i].rantime+=qflag;
						break;
					}
				}
				if(i==j)
					curthread=-1;
			}
			break;
		case PRR:
			break;
		case SRTN:
			break;
		default:
			remaining=0;
			break;
		}
		for(i=0; i<nflag; i++)
			remaining+=threads[i].runtime-threads[i].rantime;
		print_time_step(now, curthread+1);
		usleep(500000);
	}

	return 0;
}
