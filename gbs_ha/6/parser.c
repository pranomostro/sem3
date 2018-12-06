#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"

const int MAX_ARG_SZ=1024;

enum states { START, END, LITERAL, SQUOTED, DQUOTED, VAR, ESCAPED, WHITE };
typedef int Parsestate;

void parse(list_t* l, char* in, char** env)
{
	size_t i, j=0, nd, ei, cap;
	char* arg=NULL;
	Parsestate state=START, laststate=START;

	for(i=0; in[i]!='\0';)
	{
		switch(state)
		{
		case START:
			if(in[i]=='\'')
			{
				state=SQUOTED;
				i++;
			}
			else if(in[i]=='"')
			{
				state=DQUOTED;
				i++;
			}
			else if(in[i]=='\\')
			{
				laststate=LITERAL;
				state=ESCAPED;
				i++;
			}
			else if(in[i]=='$')
			{
				laststate=LITERAL;
				state=VAR;
				i++;
			}
			else if(in[i]==' '||in[i]=='\t')
			{
				state=WHITE;
				i++;
			}
			else
				state=LITERAL;
			arg=(char*)malloc(sizeof(char)*MAX_ARG_SZ);
			j=0;
			cap=MAX_ARG_SZ;
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
			else if(in[i]==' ')
			{
				state=WHITE;
				arg[j]='\0';
				list_append(l, arg);
			}
			else
				arg[j++]=in[i];
			i++;

			if(state==LITERAL&&in[i]=='\0')
			{
				state=END;
				arg[j]='\0';
				list_append(l, arg);
			}
			break;
		case SQUOTED:
			if(in[i]=='\'')
			{
				if(in[i+1]=='\0')
				{
					arg[j]='\0';
					list_append(l, arg);
					state=END;
				}
				else if(in[i+1]==' ')
				{
					arg[j]='\0';
					list_append(l, arg);
					state=WHITE;
				}
				else
					state=LITERAL;
				i++;
				break;
			}
			else if(in[i]=='\\')
			{
				laststate=SQUOTED;
				state=ESCAPED;
			}
			else if(in[i]=='$')
			{
				laststate=SQUOTED;
				state=VAR;
			}
			else
				arg[j++]=in[i];
			i++;
			break;
		case DQUOTED:
			if(in[i]=='"')
			{
				if(in[i+1]=='\0')
				{
					arg[j]='\0';
					list_append(l, arg);
					state=END;
				}
				else if(in[i+1]==' ')
				{
					arg[j]='\0';
					list_append(l, arg);
					state=WHITE;
				}
				else
					state=LITERAL;
				i++;
				break;
			}
			else if(in[i]=='\\')
			{
				laststate=DQUOTED;
				state=ESCAPED;
			}
			else if(in[i]=='$')
			{
				laststate=DQUOTED;
				state=VAR;
			}
			else
				arg[j++]=in[i];
			i++;
			break;
		case VAR:
			for(nd=0; (in[i+nd]>='A'&&in[i+nd]<='Z')||(in[i+nd]>='0'&&in[i+nd]<='9')||in[i+nd]=='_'; nd++)
				;
			if(nd==0)
			{
				perror("no variable followed $, exiting");
				exit(1);
			}
			for(ei=0; env[ei]!=NULL; ei++)
				if(!strncmp(env[ei], in+i, nd)&&env[ei][nd]=='=')
				{
					if(j+strlen(env[ei]-nd-1)>cap)
					{
						arg=realloc(arg, 2*cap);
						cap*=2;
					}
					strncpy(arg+j, env[ei]+nd+1, strlen(env[ei])-nd-1);
					j+=strlen(env[ei])-nd-1;
				}
			i+=nd;
			if(in[i]=='\0')
			{
				arg[j]='\0';
				list_append(l, arg);
			}
			state=laststate;
			break;
		case ESCAPED:
			arg[j++]=in[i];
			i++;
			if(in[i]=='\0')
			{
				arg[j]='\0';
				list_append(l, arg);
			}
			state=laststate;
			if(in[i]=='"')
			{
				i++;
				state=DQUOTED;
			}
			else if(in[i]=='\'')
			{
				i++;
				state=SQUOTED;
			}
			break;
		case WHITE:
			if(in[i]==' ')
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
			else if(in[i]=='\\')
			{
				laststate=LITERAL;
				state=ESCAPED;
				i++;
			}
			else
				state=LITERAL;
			break;
		default:
			break;
		}

	}
}
