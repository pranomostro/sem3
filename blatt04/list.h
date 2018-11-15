#include <pthread.h>

typedef struct threadContext {
    pthread_t thread;
    int n;
    int prio;
    int start; // in milliseconds, start of first thread is 0
    int target; // = k
} threadcontext_t;

struct list_elem {
    struct list_elem *next;	// Zeiger auf das naechste Element
    threadcontext_t	     *data;	// Zeiger auf ein Datenobject
};

typedef struct list {
	struct list_elem *first;// erstes Element in der Liste
	struct list_elem *last;	// letztes Element in der Liste
    int size;
} list_t;

/* function prototypes */
list_t           *list_init ();
struct list_elem *list_insert (list_t *list, threadcontext_t *data);
struct list_elem *list_append (list_t *list, threadcontext_t *data);
int              list_remove (list_t *list, struct list_elem *elem);
void             list_finit (list_t *list);
void             list_print (list_t *list, void (*print_elem) (threadcontext_t *));
struct list_elem *list_find (list_t *list, threadcontext_t *data, int (*cmp_elem) (const threadcontext_t *, const threadcontext_t *));
struct list_elem *list_getMax(list_t *list, int (*cmp_elem) (const threadcontext_t *, const threadcontext_t *));

