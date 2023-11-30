#ifndef T_MESH_H
#define T_MESH_H
#include "dev/gpu/vk_cmn.h"
#include "dev/std/std.h"

typedef enum  {
    PRIMITIVE_CUBE,
    PRIMITIVE_QUAD
} primitive_e;

typedef struct {
    uint32_t  vertex_count;
    uint32_t  index_count;
    vertex_t* vertices;
    uint32_t* indices;
} mesh_info_t;


uint32_t    mesh_new(const mesh_info_t* data);
void        mesh_bind(uint32_t id);
void        mesh_draw(uint32_t id, uint32_t instance_count);
void        mesh_del(uint32_t id);
void        mesh_clear();
void        primitive_get(enum PRIMITIVE type, mesh_info_t* out_data);


#endif // !T_MESH_H
