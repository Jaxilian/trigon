#ifndef TAC_DEF_H
#define TAC_DEF_H

#include <inttypes.h>
#include <stdbool.h>

typedef struct {
    uint32_t    vertex_count;
    uint32_t    index_count;
    void*       vertex_buffer;
    size_t      vertex_stride;
    uint32_t*   indices;
} mesh_info_t;



#endif // !TAC_DEF_H
