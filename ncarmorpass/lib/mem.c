#include <stdlib.h>

void *__carmor_malloc_(unsigned long int size)
{
	printf("This is the changed malloc of size %d", size);	
	return malloc(size);
}

void *__carmor_realloc_(void *ptr, size_t size)
{
	printf("This is the changed realloc of size %d", size);
	return realloc(ptr, size);
}
