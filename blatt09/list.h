struct memblock {
    enum {
        free_,           // signals free memory
        used            // signals used memory
    } status;
    char *addr;         // adress of the memory block
    unsigned int size;  // the size of the memory block
    unsigned int in_use;// the size of used memory in this block
};

struct list_elem {
    struct list_elem *next;	// Zeiger auf das naechste Element
    struct memblock  *data;	// Zeiger auf ein Datenobject
};

typedef struct list {
	struct list_elem *first;// erstes Element in der Liste
	struct list_elem *last;	// letztes Element in der Liste
} list_t;

/* function prototypes */
list_t           *list_init ();
struct list_elem *list_insert (list_t *list, struct memblock *data);
struct list_elem *list_append (list_t *list, struct memblock *data);
int              list_remove (list_t *list, struct list_elem *elem);
void             list_finit (list_t *list);
void             list_print (list_t *list, void (*print_elem) (struct memblock *));
struct list_elem *list_find (list_t *list, char *data, int (*cmp_elem) (const char *, const char *));
struct list_elem *list_put (list_t *list, struct list_elem *current, struct memblock *data);
