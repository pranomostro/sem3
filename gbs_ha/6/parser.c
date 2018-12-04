#include <stdlib.h>
#include <stdio.h>

#include "list.h"

const int MAX_ARG_SZ=1024;

enum states { START, END, LITERAL, SQUOTED, DQUOTED, VAR, ESCAPED, WHITE };

typedef int Parsestate;

void parse(list_t* l, char* in, char** env)
{
	size_t i, j=0;
	char* arg=NULL;
	Parsestate state=START, laststate=START;

	for(i=0; in[i]!='\0';)
	{
		printf("state: %d\t laststate: %d\t arg: \"%s\"\t in: \"%s\" \ti: %ld\t j:%ld\n", state, laststate, arg, in, i, j);
		switch(state)
		{
		case START:
			if(in[i]=='\'')
				state=SQUOTED;
			else if(in[i]=='"')
				state=DQUOTED;
			else if(in[i]=='\\')
			{
				laststate=LITERAL;
				state=ESCAPED;
			}
			else if(in[i]=='$')
			{
				laststate=LITERAL;
				state=VAR;
			}
			else if(in[i]==' '||in[i]=='\t')
				state=WHITE;
			else
				state=LITERAL;
			arg=(char*)malloc(sizeof(char)*MAX_ARG_SZ);
			j=0;
			break;
		case END:
			break;
		case LITERAL:
			if(in[i]=='"'||in[i]=='\'')
				;
			else if(in[i]=='\\')
			{
				state=ESCAPED;
				laststate=LITERAL;
			}
			else if(in[i]=='$')
			{
				state=VAR;
				laststate=LITERAL;
			}
			else if(in[i]==' '||in[i]=='\t')
				state=WHITE;
			else
				arg[j++]=in[i];
			i++;

			if(in[i]==' '||in[i]=='\t')
			{
				state=WHITE;
				list_append(l, arg);
			}
			else if(in[i]=='\0')
			{
				state=END;
				list_append(l, arg);
			}
			break;
		case SQUOTED:
			break;
		case DQUOTED:
			break;
		case VAR:
			break;
		case ESCAPED:
			break;
		case WHITE:
			break;
		default:
			break;
		}

	}
}

void parse2(list_t* l, char* in, char** env)
{
	size_t i, j=0;
	char* arg=NULL;
	Parsestate state=WHITE, laststate=WHITE;

	for(i=0; in[i]!='\0';)
	{
		printf("laststate: %d, state: %d, arg: \"%s\",\t in: \"%s\",\t i: %ld, j: %ld\n", laststate, state, arg, in+i, i, j);
		switch(state)
		{
		case LITERAL:
			if(in[i]=='\\')
			{
				laststate=LITERAL;
				state=ESCAPED;
			}
			else if(in[i+1]=='\0'||in[i+1]==' '||in[i]=='\t')
			{
				arg[j]=in[i];
				j++;
				arg[j]='\0';
				list_append(l, arg);
				state=WHITE;
			}
			else if(in[i]=='$')
				state=VAR;
			else if(in[i]=='\'')
				state=SQUOTED;
			else if(in[i]=='"')
				state=DQUOTED;
			else
			{
				arg[j]=in[i];
				j++;
			}
			i++;
			break;
		case SQUOTED:
			if(in[i]=='\\')
			{
				state=ESCAPED;
				laststate=SQUOTED;
			}
			else if(in[i]=='\'')
			{
				if(in[i+1]!=' '&&in[i+1]!='\t'&&in[i+1]!='\0')
				{
					state=LITERAL;
					goto ends;
				}
				arg[j]='\0';
				list_append(l, arg);
				state=WHITE;
			}
			else
			{
				arg[j]=in[i];
				j++;
			}
			ends:
			i++;
			break;
		case DQUOTED:
			if(in[i]=='\\')
			{
				state=ESCAPED;
				laststate=DQUOTED;
			}
			else if(in[i]=='"')
			{
				if(in[i+1]!=' '&&in[i+1]!='\t'&&in[i+1]!='\0')
				{
					state=LITERAL;
					goto endd;
				}
				arg[j]='\0';
				list_append(l, arg);
				state=WHITE;
			}
			else
			{
				arg[j]=in[i];
				j++;
			}
			endd:
			i++;
			break;
		case ESCAPED:
			arg[j]=in[i];
			i++;
			j++;
			if(in[i+1]=='\0')
			{
				i++;
				arg[j]='\0';
				list_append(l, arg);
				state=laststate;
				break;
			}
			state=laststate;
			break;
		case WHITE:
			if(in[i]==' '||in[i]=='\t')
			{
				i++;
				break;
			}
			arg=(char*)malloc(sizeof(char)*MAX_ARG_SZ);
			j=0;
			if(in[i]=='"')
			{
				state=DQUOTED;
				i++;
			}
			else if(in[i]=='\'')
			{
				state=SQUOTED;
				i++;
			}
			else if(in[i]=='$')
			{
				state=VAR;
				i++;
			}
			else if(in[i]=='\\')
			{
				state=ESCAPED;
				i++;
			}
			else
				state=LITERAL;
			break;
		case VAR:
			break;
		default:
			break;
		}
	}
}
