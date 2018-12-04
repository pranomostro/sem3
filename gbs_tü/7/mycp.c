#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "arg.h"

char* argv0;

static void usage(void)
{
	fprintf(stderr, "%s [-i infile] [-o outfile] [filters]\n", argv0);
	exit(1);
}

int main(int argc, char** argv)
{
	int infd, outfd;
	size_t len;
	char buf[1024];
	char* infile=NULL, * outfile=NULL;

	ARGBEGIN
	{
	case 'i':
		infile=EARGF(usage);
		break;
	case 'o':
		outfile=EARGF(usage);
		break;
	default:
		usage();
		break;
	}
	ARGEND;

	if(infile==NULL)
		infd=0;
	else
		infd=open(infile, O_RDONLY);

	if(outfile==NULL)
		outfd=1;
	else
		outfd=open(outfile, O_WRONLY|O_CREAT|O_TRUNC, 0600);

	if(infd<0||outfd<0)
	{
		fprintf(stderr, "could not open input or output file, exiting\n");
		exit(1);
	}

	while((len=read(infd, buf, sizeof(buf)))>0)
		write(outfd, buf, len);

	return 0;
}
