#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "list.h"

#define IN_SZ 1024

typedef struct prc {
	int p;
}process;

int cmppid(const char* p1, const char* p2)
{
	return ((process*)p1)->p-((process*)p2)->p;
}

const char* prompt="$ ";
const char* pathvar="PATH=";

void parse(list_t*, char*, char**);
void print_string(char* s);

void print_string(char* s)
{
	puts(s);
}

int main(int argc, char** argv, char** envp)
{
	size_t s, t;
	char* tknd, * in=(char*)malloc(sizeof(char)*IN_SZ), * cmd=(char*)malloc(sizeof(char)*IN_SZ);
	char** split, ** paths=(char**)malloc(sizeof(char*)*IN_SZ);
	pid_t p;
	struct list_elem* le;
	process retproc, * child;
	list_t* parsed=list_init(), * prcs=list_init();

	for(s=0; envp[s]!=NULL; s++)
		if(!strncmp(pathvar, envp[s], strlen(pathvar)))
		{
			t=0;
			tknd=strtok(envp[s]+strlen(pathvar), ":");
			paths[t++]=tknd;
			while((tknd=strtok(NULL, ":"))!=NULL)
				paths[t++]=tknd;
		}

	paths[t]=NULL;

	printf("%s", prompt);
	fflush(stdout);
	fgets(in, IN_SZ, stdin);

	while(!feof(stdin))
	{
		if(in[strnlen(in, IN_SZ)-1]=='\n')
			in[strnlen(in, IN_SZ)-1]='\0';

		parse(parsed, in, envp);
		split=list_to_array(parsed);

		if(split[0]==NULL)
			goto noexec;

		if(!strncmp(split[0], "exit", strlen("exit")))
			exit(0);

		switch((p=fork()))
		{
		case 0:
			for(s=0; paths[s]!=NULL; s++)
			{
				if(!strncmp(split[0], "./", 2)||!strncmp(split[0], "../", 3))
					strncpy(cmd, split[0], strlen(split[0])+1);
				else
				{
					strncpy(cmd, paths[s], IN_SZ);
					strncpy(cmd+strlen(paths[s]), "/", 2);
					strncpy(cmd+strlen(paths[s])+1, split[0], strlen(split[0]));
					strncpy(cmd+strlen(paths[s])+1+strlen(split[0]), "\0", 1);
				}
				/*
					If split ends with "> f1" or "< f2" or "< f1 > f2" or "> f2 < f1"
				*/
				execve(cmd, split, envp);
			}
			perror("command not found");
			exit(1);
			break;
		case -1:
			perror("could not fork, exiting");
			exit(1);
			break;
		default:
			child=(process*)malloc(sizeof(process));
			child->p=p;
			list_append(prcs, (char*) child);
			break;
		}

		p=wait(NULL);
		retproc.p=p;
		le=list_find(prcs, (char*) &retproc, cmppid);
		free(le->data);
		list_remove(prcs, le);

	noexec:

		free(split);
		list_finit(parsed);

		printf("%s", prompt);
		fflush(stdout);
		fgets(in, IN_SZ, stdin);
	}

	return 0;
}
