#include <stdlib.h>
#include <stdio.h>

#include "list.h"

list_t *list_init() {
	printf("calling list_init\n");

	list_t *l=malloc(sizeof(list_t));

	if(l==NULL)
		return NULL;

	l->first=NULL;
	l->last=NULL;
	return l;
}

struct list_elem *list_insert(list_t *list, char *data) {
	printf("callist list_insert\n");

	struct list_elem *le=malloc(sizeof(struct list_elem));

	if(le==NULL)
		return NULL;

	le->data=data;
	le->next=list->first;
	list->first=le;

	if(list->last==NULL)
		list->last=list->first;

	return le;
}

struct list_elem *list_append(list_t *list, char *data) {
	printf("calling list_append\n");

	struct list_elem *le=malloc(sizeof(struct list_elem));

	if(le==NULL)
		return NULL;

	le->data=data;
	le->next=NULL;

	if(list->first==NULL)
		list->first=le;

	if(list->last==NULL) {
		list->last=le;
		return le;
	}

	list->last->next=le;
	list->last=le;

	return le;
}

int list_remove(list_t *list, struct list_elem *elem) {
	printf("calling list_append\n");

	struct list_elem *le, *nel;

	if(elem==list->first) {
		le=list->first->next;
		free(list->first);
		list->first=le;
		return 0;
	}

	for(le=list->first; le->next!=NULL; le=le->next)
		if(le->next==elem) {
			nel=le->next;
			le->next=nel->next;
			free(nel);
			return 0;
		}

	return -1;
}

void list_print(list_t *list, void (*print_elem) (char *)) {
	printf("calling list_print\n");

	int i;
	struct list_elem *le;

	if(list->first==NULL)
		return;

	for(i=1, le=list->first; le!=NULL; le=le->next, i++) {
		printf("%d:", i);
		print_elem(le->data);
	}
}
