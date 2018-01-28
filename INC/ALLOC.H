#ifndef ALLOC_H
#define ALLOC_H

#define MEM_OK		1
#define MEM_ERR	0

int alloc_mem(void **, int);
void free_mem(void *);

#endif

