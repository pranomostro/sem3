#include <stdlib.h>
#include <stdio.h>

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

	if(l==NULL)
		return NULL;

	l->first=NULL;
	l->last=NULL;
	return l;
}

struct list_elem *list_insert(list_t *list, struct memblock *data) {
	struct list_elem *le=malloc(sizeof(struct list_elem));

	if(le==NULL)
		return NULL;

	le->data = data;
	le->next = list->first;
	list->first = le;

	if(list->last==NULL)
		list->last = list->first;

	return le;
}

struct list_elem *list_append(list_t *list, struct memblock *data) {
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
	struct list_elem *le, *nel;

	if(list->first==NULL)
		return -1;

	if(elem==list->first) {
		if(elem==list->last)
			list->last=NULL;
		le=list->first->next;
		free(list->first);
		list->first=le;
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
			return 0;
		}

	return -1;
}

struct list_elem *list_find(list_t *list, char *data, int (*cmp_elem) (const char *, const char *)) {
	struct list_elem *le;
	for(le = list->first; le != NULL; le = le->next)
		if(cmp_elem(le->data->addr, data) == 0)
			return le;
	return NULL;
}

void list_finit(list_t *list) {
	while(list->first!=NULL)
		list_remove(list, list->first);
}

void list_print(list_t *list, void (*print_elem) (struct list_elem *)) {
	int i;
	struct list_elem *le;

	if(list->first==NULL)
		return;

	for(i=1, le=list->first; le!=NULL; le=le->next, i++) {
		printf("%d:", i);
		print_elem(le);
	}
}

struct list_elem *list_put(list_t *list, struct list_elem *current, struct memblock *data) {
	struct list_elem *temp = list->first;
	for( ; temp != NULL ; temp = temp->next) {
		if (temp->data == current->data) {
			if (temp->next != NULL) {
				struct list_elem *next = temp->next;
				struct list_elem *elem = malloc(sizeof(struct list_elem));
				elem->data = data;
				elem->next = next;
				temp->next = elem;
				return elem;
			} else {
				return list_append(list, data);
			}
		}
	}
	return NULL;
}

struct list_elem *list_nth (list_t *list, unsigned int index) {
	struct list_elem *elem = list->first;
	int i = 0;
	while(elem != NULL && i < index) {
		elem = elem->next;
	}
	return elem;
}