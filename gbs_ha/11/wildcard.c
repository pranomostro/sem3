#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int wildcard(char*** split)
{
	size_t r, s, t;
	char** expanded=(char**)malloc(BUFSIZ*sizeof(char*));
	glob_t g;

	for(s=r=0; (*split)[s]!=NULL; s++)
	{
		glob((*split)[s], 0, NULL, &g);
		if(g.gl_pathc==0)
		{
			expanded[r]=(char*)malloc(strlen((*split)[s])*sizeof(char));
			strcpy(expanded[r], (*split)[s]);
			r++;
			continue;
		}
		for(t=0; t<g.gl_pathc; t++)
		{
			expanded[r]=(char*)malloc(strlen(g.gl_pathv[t])*sizeof(char));
			strcpy(expanded[r], g.gl_pathv[t]);
			r++;
		}
		globfree(&g);
	}

	expanded[r]=NULL;

	*split=expanded;

	return 0;
}
