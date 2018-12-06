#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

#define IN_SZ 1024

const char* prompt="$ ";

void parse(list_t*, char*, char**);
void print_string(char* s);

void print_string(char* s)
{
	puts(s);
}

int main(int argc, char** argv, char** envp)
{
	char* in=(char*)malloc(sizeof(char)*IN_SZ);
	list_t* parsed=list_init();

	printf("%s", prompt);
	fgets(in, IN_SZ, stdin);

	while(!feof(stdin))
	{
		if(in[strnlen(in, IN_SZ)-1]=='\n')
			in[strnlen(in, IN_SZ)-1]='\0';

		parse(parsed, in, envp);

		list_print(parsed, print_string);

		if(!strncmp(parsed->first->data, "exit", strlen("exit")))
			exit(0);

		list_finit(parsed);

		printf("%s", prompt);
		fgets(in, IN_SZ, stdin);
	}

	return 0;
}
