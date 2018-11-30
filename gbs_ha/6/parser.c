#include <stdlib.h>
#include <stdio.h>

#include "list.h"

const int MAX_ARG_SZ=1024;

enum states { LITERAL, SQUOTED, DQUOTED, VAR, LESCAPED, SQESCAPED, DQESCAPED, WHITE };

typedef int Parsestate;

void parse(list_t* l, char* in, char** env)
{
	size_t i, j=0;
	char* arg=NULL;
	Parsestate state=WHITE;

	for(i=0; in[i]!='\0';)
	{
		printf("state: %d, arg: \"%s\",\t in: \"%s\",\t i: %ld, j: %ld\n", state, arg, in+i, i, j);
		switch(state)
		{
		case LITERAL:
			if(in[i]=='\\')
				state=LESCAPED;
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
				state=SQESCAPED;
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
				state=DQESCAPED;
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
		case LESCAPED:
			arg[j]=in[i];
			if(in[i+1]=='\0'||in[i+1]==' '||in[i]=='\t')
			{
				arg[j]=in[i];
				j++;
				arg[j]='\0';
				list_append(l, arg);
				state=WHITE;
				break;
			}
			i++;
			j++;
			state=LITERAL;
			break;
		case SQESCAPED:
			arg[j]=in[i];
			i++;
			j++;
			state=SQUOTED;
			break;
		case DQESCAPED:
			arg[j]=in[i];
			i++;
			j++;
			state=DQUOTED;
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
			else
				state=LITERAL;
			break;
		case VAR:
		default:
			break;
		}
	}
}
