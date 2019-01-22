#include <termios.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

struct termios term_original;
int term_fd;

void catch(int signum)
{
	if(signum==SIGINT)
	{
		tcsetattr(term_fd, TCSANOW, &term_original);
		exit(1);
	}
}

int main()
{
	struct termios term;
	memset((char*)&term, 0, sizeof(term));
	term_fd=fileno(stdin);
	tcgetattr(term_fd, &term);
	if(tcgetattr(term_fd, &term)==-1)
	{
		perror("Could not get terminal attributes");
		exit(1);
	}
	term_original=term;
	signal(SIGINT, catch);
	term.c_lflag&=~(ICANON|ECHO);
	term.c_cc[VMIN]=1;
	term.c_cc[VTIME]=0;

	tcsetattr(term_fd, TCSANOW, &term);

	char c;
	while((c=getchar())!=EOF)
	{
		printf("Get char %c with hex-value %x\n", c, c);
	}

	tcsetattr(term_fd, TCSANOW, &term_original);

	return 0;
}
