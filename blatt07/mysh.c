#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "list.h"

#define IN_SZ 1024

const char* exit_cmd="exit";
const char* prompt="$ ";
const char* pathvar="PATH=";
const char* thisdir="./";
const char* updir="../";
const char* rootdir="/";
const char* into=">";
const char* outof="<";

typedef struct prc {
	int p;
}process;

int cmppid(const char* p1, const char* p2)
{
	return ((process*)p1)->p-((process*)p2)->p;
}

void parse(list_t*, char*, char**);
void print_string(char* s);

void print_string(char* s)
{
	puts(s);
}

void redirect(char** split)
{
	int nsi, nso;
	size_t t;
	char* intof=NULL, * outoff=NULL;
	for(t=0; split[t]!=NULL; t++)
		;
	if(t>=5)
	{
		if(!strncmp(split[t-4], outof, strlen(outof))&&
		   !strncmp(split[t-2], into, strlen(into)))
			outoff=split[t-3], intof=split[t-1];
	}
	if(t>=3)
	{
		if(!strncmp(split[t-2], outof, strlen(outof)))
			outoff=split[t-1];
		else if(!strncmp(split[t-2], into, strlen(into)))
			intof=split[t-1];
	}
	if(outoff!=NULL&&intof!=NULL)
	{
		close(0);
		nsi=open(outoff, O_RDONLY);
		if(nsi<0)
		{
			perror("could not redirect stdin, exiting");
			exit(1);
		}
		close(1);
		nso=open(intof, O_WRONLY|O_CREAT, 0600);
		if(nso<0)
		{
			perror("could not redirect stdout, exiting");
				exit(1);
		}
		split[t-4]=NULL;
	}
	else if(outoff!=NULL)
	{
		close(0);
		nsi=open(outoff, O_RDONLY);
		if(nsi<0)
		{
			perror("could not redirect stdin, exiting");
			exit(1);
		}
		split[t-2]=NULL;
	}
	else if(intof!=NULL)
	{
		close(1);
		nso=open(intof, O_WRONLY|O_CREAT|O_TRUNC, 0600);
		if(nso<0)
		{
			perror("could not redirect stdout, exiting");
			exit(1);
		}
		split[t-2]=NULL;
	}
}

int main(int argc, char** argv, char** envp)
{
	size_t s, t;
	char* tknd, * in, * cmd;
	char** split, ** paths;
	pid_t p;
	struct list_elem* le;
	process retproc, * child;
	list_t* parsed, * prcs;

	in=(char*)malloc(sizeof(char)*IN_SZ);
	cmd=(char*)malloc(sizeof(char)*IN_SZ);

	paths=(char**)malloc(sizeof(char*)*IN_SZ);

	parsed=list_init();
	prcs=list_init();

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

		if(!strncmp(split[0], exit_cmd, strlen(exit_cmd)))
			exit(0);

		switch((p=fork()))
		{
		case 0:
			for(s=0; paths[s]!=NULL; s++)
			{
				if(!strncmp(split[0], thisdir, strlen(thisdir))||!strncmp(split[0], updir, strlen(updir)))
					strncpy(cmd, split[0], strlen(split[0])+1);
				else
				{
					strncpy(cmd, paths[s], IN_SZ);
					strncpy(cmd+strlen(paths[s]), rootdir, strlen(rootdir));
					strncpy(cmd+strlen(paths[s])+1, split[0], strlen(split[0]));
					strncpy(cmd+strlen(paths[s])+1+strlen(split[0]), "\0", 1);
				}
				redirect(split);
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

	free(in);
	free(cmd);
	free(paths);
	list_finit(parsed);
	list_finit(prcs);

	free(parsed);
	free(prcs);

	return 0;
}
