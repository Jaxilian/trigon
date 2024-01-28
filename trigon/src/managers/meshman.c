#include "meshman.h"
#include <vstd/vfs.h>
#include <types/mesh.h>
#include <vkl/common.h>
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

static bool import_mesh(cgltf_mesh* mesh) {
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
        vertex3_t* vertex_buffer = (vertex3_t*)malloc(sizeof(vertex3_t) * vertex_count);

        if (!vertex_buffer) {
            printf("failed to allocate memory for mesh %s vertex buffer\n", mesh->name);
            return false;
        }

        for (cgltf_size v = 0; v < vertex_count; ++v) {
            cgltf_accessor_read_float(positions, v, vertex_buffer[v].position, 3);

            if (normals) {
                cgltf_accessor_read_float(normals, v, vertex_buffer[v].normal, 3);
            }
            else {
                glm_vec3_zero(vertex_buffer[v].normal);
            }

            if (texcoords) {
                cgltf_accessor_read_float(texcoords, v, vertex_buffer[v].uv, 2);
            }
            else {
                glm_vec2_zero(vertex_buffer[v].uv);
            }

            // If the color attribute is not present in the glTF model,
            // you can set a default color here. For example:
            glm_vec3_one(vertex_buffer[v].color); // white color
        }

        size_t index_count = indices->count;
        uint32_t* index_buffer = (uint32_t*)malloc(sizeof(uint32_t) * index_count);

        if (!index_buffer) {
            printf("failed to allocate memory for mesh %s index buffer\n", mesh->name);
            free(vertex_buffer);
            return false;
        }

        for (cgltf_size idx = 0; idx < index_count; ++idx) {
            cgltf_accessor_read_uint(indices, idx, &index_buffer[idx], 1);
        }

        mesh_info_t mesh_info = {
            .vertex_count = (uint32_t)vertex_count,
            .index_count = (uint32_t)index_count,
            .vertex_buffer = (void*)vertex_buffer,
            .vertex_stride = sizeof(vertex3_t),
            .indices = index_buffer
        };

        uint32_t mesh_id = mesh_new(&mesh_info);

        free(vertex_buffer);
        free(index_buffer);

        #ifdef _DEBUG
        printf("[ID: %d] imported mesh: %s\n", mesh_id, mesh->name);
        #endif
    }

    return true;
}

bool meshman_import(const char* path) {
	path_os_t p = { 0 };
	path_new(&p, path);

	const char* ext = path_get_ext(&p);

	cgltf_options options = { 0 };
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse_file(&options, path, &data);

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

	for (cgltf_size i = 0; i < data->meshes_count; ++i) {
		cgltf_mesh* mesh = &data->meshes[i];
		if (!import_mesh(mesh)) {
			printf("failed to import mesh %s!\n", mesh->name);
			cgltf_free(data);
			return false;
		}
	}

	cgltf_free(data);
	return true;
}


