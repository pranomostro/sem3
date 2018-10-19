#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	size_t i;
	ssize_t rs;
	char in[BUFSIZ];
	char *ia="Eingabe: ", *oa="Ausgabe: ", *greet="Hallo: ", *sep=" -- ";

	write(1, ia, strlen(ia));

	rs=read(0, in, BUFSIZ);

	if(rs<0)
	{
		fprintf(stderr, "could not read from stdin, exiting\n");
		exit(1);
	}

	in[rs]='\0';

	if(in[rs-1]=='\n')
	{
		in[rs-1]='\0';
		rs--;
	}

	write(1, oa, strlen(oa));
	write(1, greet, strlen(greet));
	write(1, in, rs);
	write(1, sep, strlen(sep));

	for(i=0; i<rs&&isalpha(in[i]); i++)
		in[i]=toupper(in[i]);

	in[i]='\0';

	for(i=0; in[i]!='\0'; i++)
	{
		if(in[i]>='A'&&in[i]<='M')
			in[i]+=13;
		else
			in[i]-=13;
	}

	write(1, in, i);
	write(1, "\n", 1);

	return 0;
}
