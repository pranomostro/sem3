#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "list.h"

void print_string(char *s) {
	puts(s);
}

int main(int argc, char **argv, char **envp) {
	int i;
	list_t *li;

	if((li=list_init())==NULL) {
		perror("Cannot allocate memory");
		exit(-1);
	}

	for(i=1; i<argc; i++) {
		if(argv[i][0]=='-'&&argv[i][2]=='\0') {
			if(i+1>=argc) {
				perror("argument expected for flag");
				exit(-1);
			}
			switch(argv[i][1]) {
				case 'a': list_append(li, argv[i+1]); break;
				case 'i': list_insert(li, argv[i+1]); break;
				case 'r': list_remove(li, list_find(li, argv[i+1], strcmp)); break;
				default: perror("unknown flag"); exit(-1);
			}
			i++;
		} else {
			perror("-[air] expected");
			exit(-1);
		}
	}

	list_print(li, print_string);
	list_finit(li);
	exit(0);
}
