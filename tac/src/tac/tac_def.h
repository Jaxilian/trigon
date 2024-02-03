#ifndef TAC_DEF_H
#define TAC_DEF_H

#include <vstd/vfs.h>
#include <inttypes.h>
#include <stdbool.h>

typedef struct {
    char        name[64];
    uint32_t    vertex_count;
    uint32_t    index_count;
    void*       vertex_buffer;
    size_t      vertex_stride;
    uint32_t*   indices;
} mesh_info_t;

typedef struct {
    size_t       size;
    uint32_t     mesh_count;
    mesh_info_t* mesh_buffer;
} tac_gltf_info_t;


#endif // !TAC_DEF_H
