#include "tac_def.h"
#include "vkl/vkl.h"

typedef struct trigon {
    bool            initialized;
    uint32_t        vertex_count;
    uint32_t        index_count;
    vkl_buffer_t    buffer;
    size_t          index_offset;
} mesh_t;

