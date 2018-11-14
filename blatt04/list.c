#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "list.h"

/* For short lists:
	0 elements:
		l->first=NULL;
		l->last=NULL;
	1 element:
		l->first==l->last==elem;
		elem->next=NULL;
	2 elements:
		l->first=elem1;
		l->last=elem2;
		elem1->next=elem2;
		elem2->next=NULL;
*/

list_t *list_init() {
	list_t *l=malloc(sizeof(list_t));

	l->size = 0;
	if(l==NULL)
		return NULL;

	l->first=NULL;
	l->last=NULL;
	return l;
}

struct list_elem *list_insert(list_t *list, threadcontext_t *data) {
	struct list_elem *le=malloc(sizeof(struct list_elem));

	if(le==NULL)
		return NULL;

	le->data=data;
	le->next=list->first;
	list->first=le;

	if(list->last==NULL)
		list->last=list->first;

	list->size++;
	return le;
}

struct list_elem *list_append(list_t *list, threadcontext_t *data) {
	struct list_elem *le=malloc(sizeof(struct list_elem));

	if(le==NULL)
		return NULL;

	le->data=data;
	le->next=NULL;

	if(list->first==NULL)
		list->first=le;

	if(list->last==NULL) {
		list->last=le;
		list->size++;
		return le;
	}

	list->last->next=le;
	list->last=le;
	list->size++;

	return le;
}

int list_remove(list_t *list, struct list_elem *elem) {
	struct list_elem *le, *nel;

	if(list->first==NULL)
		return -1;

	if(elem==list->first) {
		if(elem==list->last)
			list->last=NULL;
		le=list->first->next;
		free(list->first);
		list->first=le;
		list->size--;
		return 0;
	}

	for(le=list->first; le->next!=NULL; le=le->next)
		if(le->next==elem) {
			nel=le->next;
			if(nel==list->last) {
				le->next=NULL;
				list->last=le;
			} else {
				le->next=nel->next;
			}
			free(nel);
			list->size--;
			return 0;
		}

	return -1;
}

struct list_elem *list_find(list_t *list, threadcontext_t *data, int (*cmp_elem) (const threadcontext_t *, const threadcontext_t *)) {
	struct list_elem *le;

	for(le=list->first; le!=NULL; le=le->next)
		if(cmp_elem(le->data, data)==0)
			return le;
	return NULL;
}

void list_finit(list_t *list) {
	while(list->first!=NULL)
		list_remove(list, list->first);
}

void list_print(list_t *list, void (*print_elem) (threadcontext_t *)) {
	int i;
	struct list_elem *le;

	if(list->first==NULL)
		return;

	for(i=1, le=list->first; le!=NULL; le=le->next, i++) {
		printf("%d:", i);
		print_elem(le->data);
	}
}

int list_getHighestPrio(list_t *list, int (*cmp_elem) (const threadcontext_t *, const threadcontext_t *)) {
	struct list_elem *next = list->first;
	struct list_elem *highest = list->first;
	for( ; next != NULL; next = next->next) {
		if(cmp_elem(highest->data, next->data) > 0) {
			highest = next;
		}
	}
	return highest->data->prio;
}
