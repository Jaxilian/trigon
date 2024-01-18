#ifndef VSTD_CPOOL_H
#define VSTD_CPOOL_H

#include <memory.h>
#include <stdlib.h>
#include <inttypes.h>
#include "debug.h"

typedef struct {
	uint32_t	max_count;
	uint32_t	length; // amount of occupied slots
	size_t		stride;
	void*		data;

	uint32_t	queue_out_ptr;
	uint32_t	queue_in_ptr;
	uint32_t* queue;
} cpool_t;

void		cpool_new(cpool_t* pool, size_t stride, uint32_t preset_length);
uint32_t	cpool_add(cpool_t* pool, void* data);
uint32_t	cpool_len(cpool_t* pool);
inline
void*		cpool_get(cpool_t* pool, uint32_t index);
void		cpool_pop(cpool_t* pool, uint32_t index);
void		cpool_del(cpool_t* pool);
size_t		cpool_mem(cpool_t* pool);

#endif // !CPOOL_H
