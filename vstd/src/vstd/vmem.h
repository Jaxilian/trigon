#ifndef VSTD_VMEM_H
#define VSTD_VMEM_H

#include <memory.h>
#include <stdlib.h>
#include <inttypes.h>

#ifdef _DEBUG
void* cmem_alloc(size_t size, const char* file, uint32_t line);
void  cmem_free(void* ptr);
void  cmem_check();

#define mem_alloc(size) cmem_alloc(size,  __FILE__, __LINE__);
#define mem_free cmem_free
#define mem_check cmem_check
#else
#define mem_alloc(size)malloc(size)
#define mem_free(ptr)free(ptr)
#define mem_check()
#endif


#endif