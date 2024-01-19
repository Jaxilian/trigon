#include "mesh.h"
#include <inttypes.h>
/*
static vertex_t cube_vertices[] = {
    {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},{0.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
};

static uint32_t cube_indices[] = {
    0, 1, 2, 2, 3, 0,   // Front
    4, 5, 6, 6, 7, 4,   // Back
    4, 0, 3, 3, 7, 4,   // Left
    1, 5, 6, 6, 2, 1,   // Right
    3, 2, 6, 6, 7, 3,   // Top
    0, 1, 5, 5, 4, 0    // Bottom
};

static vertex_t quad_vertices[] = {
    {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

static uint32_t quad_indices[] = {
    0, 1, 2,
    2, 3, 0
};

void primitive_get(enum PRIMITIVE type, mesh_info_t* out_data) {
    switch (type) {
    case PRIMITIVE_CUBE:
        out_data->index_count = 36;
        out_data->indices = cube_indices;
        out_data->vertex_count = 8;
        out_data->vertices = cube_vertices;
        break;
    case PRIMITIVE_QUAD:
        out_data->index_count = 6;
        out_data->indices = quad_indices;
        out_data->vertex_count = 4;
        out_data->vertices = quad_vertices;
        break;
    default:
        out_data->index_count = 0;
        out_data->indices = NULL;
        out_data->vertex_count = 0;
        out_data->vertices = NULL;
        return;
    }
}
*/