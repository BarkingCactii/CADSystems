/*----------------------------------------------------------------------------
alloc.c

General memory allocation routines
by Jeff Hill
----------------------------------------------------------------------------*/

#include <stdlib.h>
#include "alloc.h"

#define ELEM_SZ	1
#define NO_MEMORY	"Out of memory."

int alloc_mem(void **alloc_ptr, int num_bytes)
{
	*alloc_ptr = calloc(ELEM_SZ, num_bytes);

	if (*alloc_ptr == (void *)NULL)
		return(MEM_ERR);

	return(MEM_OK);
}

void free_mem(void *alloc_ptr)
{
	if (alloc_ptr)
		free(alloc_ptr);
}


