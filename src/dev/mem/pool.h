#ifndef T_MEM_POOL_H
#define T_MEM_POOL_H
#include "dev/std/std.h"

typedef struct {
	uint32_t	count;
	size_t		stride;
	size_t		size;
	void*		data;
} mem_pool_t;

bool	mem_pool_new(size_t stride, uint32_t count,mem_pool_t* out);
extern inline
void*	pool_get(mem_pool_t* in, size_t index);
void	pool_set(mem_pool_t* in, size_t index, void* data);
void	pool_pop(mem_pool_t* in, size_t index);
void	pool_del(mem_pool_t* pool );

#endif // !T_POOL_H
