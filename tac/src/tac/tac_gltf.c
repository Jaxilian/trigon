#include "tac.h"
#include "tac_def.h"

#include <stdlib.h>
#include <stdio.h>
#include <cglm/cglm.h>
#include "vkl/common.h"
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"


static bool import_mesh(cgltf_mesh* mesh, tac_gltf_info_t* out, uint32_t out_idx) {
    for (cgltf_size j = 0; j < mesh->primitives_count; ++j) {
        cgltf_primitive* primitive = &mesh->primitives[j];

        if (primitive->type != cgltf_primitive_type_triangles) {
            continue;
        }

        cgltf_accessor* positions = NULL;
        cgltf_accessor* normals = NULL;
        cgltf_accessor* texcoords = NULL;
        cgltf_accessor* indices = primitive->indices;

        for (cgltf_size k = 0; k < primitive->attributes_count; ++k) {
            cgltf_attribute* attr = &primitive->attributes[k];
            if (attr->type == cgltf_attribute_type_position) {
                positions = attr->data;
            }
            else if (attr->type == cgltf_attribute_type_normal) {
                normals = attr->data;
            }
            else if (attr->type == cgltf_attribute_type_texcoord) {
                texcoords = attr->data;
            }
        }

        if (!positions || !indices) {
            printf("gltf error: required attribute in mesh %s was missing!\n", mesh->name);
            return false;
        }

        size_t vertex_count = positions->count;
        size_t index_count = indices->count;

        vertex3_t* vertices = (vertex3_t*)malloc(sizeof(vertex3_t) * vertex_count);
        uint32_t* index_buffer = (uint32_t*)malloc(sizeof(uint32_t) * index_count);

        for (cgltf_size v = 0; v < vertex_count; ++v) {
            float pos[3], nor[3], uv[2];
            cgltf_accessor_read_float(positions, v, pos, 3);

            if (normals) {
                cgltf_accessor_read_float(normals, v, nor, 3);
            }

            if (texcoords) {
                cgltf_accessor_read_float(texcoords, v, uv, 2);
            }

            glm_vec3_copy((vec3) { pos[0], pos[1], pos[2] }, vertices[v].position);

            if (normals) {
                glm_vec3_copy((vec3) { -nor[0], -nor[1], -nor[2] }, vertices[v].normal);
            }
            if (texcoords) {
                glm_vec2_copy((vec2) { uv[0], uv[1] }, vertices[v].uv);
            }
            glm_vec3_copy(vertices[v].normal, vertices[v].color);
        }

        for (cgltf_size idx = 0; idx < index_count; ++idx) {
            uint32_t index;
            cgltf_accessor_read_uint(indices, idx, &index, 1);
            index_buffer[idx] = index;
            //printf("Index %zu: %u\n", idx, index);
        }


        out->mesh_buffer[out_idx] = (mesh_info_t){
            .vertex_buffer = vertices,
            .vertex_stride = (size_t)sizeof(vertex3_t),
            .index_count = (uint32_t)index_count,
            .vertex_count = (uint32_t)vertex_count,
            .indices = index_buffer
        };

        strcpy(out->mesh_buffer[out_idx].name, mesh->name);


#ifdef _DEBUG
        //uint32_t mesh_id = mesh_new(&info);
        //printf("[ID: %d] imported mesh: %s\n", mesh_id, mesh->name);
#endif
    }

    return true;
}

bool tac_read_gltf(const char* path, tac_gltf_info_t* out) {

    if (!out) {
        printf("tac_read_gltf -> out was NULL!\n");
        return false;
    }

    path_os_t p = { 0 };
    path_new( &p, path );

    const char* ext = path_get_ext(&p);

    cgltf_options   options = { 0 };
    cgltf_data*     data    = NULL;
    cgltf_result    result  = cgltf_parse_file(&options, path, &data);

    if (result != cgltf_result_success) {
        printf("failed to import %s\n", path);
        return false;
    }

    result = cgltf_load_buffers(&options, data, path);
    if (result != cgltf_result_success) {
        printf("failed to load gltf buffers\n");
        cgltf_free(data);
        return false;
    }

    if (out->mesh_buffer) {
        free(out->mesh_buffer);
        out->mesh_buffer = NULL;
        out->mesh_count = 0;
        out->size = 0;
    }

    out->mesh_count = (uint32_t)data->meshes_count;
    out->size = sizeof(mesh_info_t) * data->meshes_count;
    out->mesh_buffer = malloc(out->size);

    for (cgltf_size i = 0; i < data->meshes_count; ++i) {
        cgltf_mesh* mesh = &data->meshes[i];
        if (!import_mesh(mesh, out, (uint32_t)i)) {
            printf("failed to import mesh %s!\n", mesh->name);
            cgltf_free(data);
            return false;
        }
    }

    cgltf_free(data);
    return true;
}

