enum mem_algo {
    none,
    first,
    worst
};

int mem_init (unsigned int size, unsigned int blocksize, enum mem_algo strategy);
void *mem_alloc (unsigned int size);
void mem_remove (unsigned int index);
void mem_free (void *addr);
void mem_dump ();

