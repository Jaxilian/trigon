#ifndef T_MEM_H
#define T_MEM_H
#include "dev/std/std.h"

typedef struct {
	uint32_t	max_count;
	uint32_t	length;
	size_t		stride;
	ptr_t		data;

	uint32_t	queue_out_ptr;
	uint32_t	queue_in_ptr;
	uint32_t*	queue;
} cpool_t;

#ifdef _DEBUG
ptr_t cmem_alloc(size_t size, cstr_t file, uint32_t line);
void  cmem_free(ptr_t ptr);
void  cmem_check();

#define mem_alloc(size) cmem_alloc(size,  __FILE__, __LINE__);
#define mem_free cmem_free
#define mem_check cmem_check
#else
#define mem_alloc(size)malloc(size)
#define mem_free(ptr)free(ptr)
#define mem_check
#endif
#endif // !T_MEM_H
