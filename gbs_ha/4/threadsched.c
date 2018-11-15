#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

enum Algorithm { RR, PRR, SRTN };

struct Process
{
	int num;
	int prio;
	int start;
	int runtime;
	int rantime;
};

typedef enum Algorithm Algorithm;
typedef struct Process Process;

int main(int argc, char** argv)
{
	char c;
	int nflag, tflag, qflag;
	Algorithm aflag;
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

	threads=(Process*)malloc(nflag*sizeof(Process));

	return 0;
}
