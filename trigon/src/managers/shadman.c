#include "shadman.h"
#include <stdbool.h>
#include <vkl/vkl.h>
#include <vkl/common.h>
#include "core/core.h"
#include <vstd/vfs.h>
#include "shaders/shader_global.h"
#include "types/mesh.h"
typedef struct {
	vkl_pipeline_t          pipeline;
    vkl_descriptor_set_t    sets;
    vkl_buffer_t            matrix_buffer;
    uint32_t                matrix_count;
    mat4                    matrices;
} shader_container_t;

static shader_container_t simple_shader = { 0 };

void shader_simple_del() {
    if (simple_shader.matrix_buffer.initialized) {
        vkl_buffer_del(&simple_shader.matrix_buffer);
    }

    if (simple_shader.sets.initialized) {
        vkl_descriptor_set_del(trigon_core_vkl_device(), &simple_shader.sets);
    }

    if (simple_shader.pipeline.initialized) {
        vkl_pipeline_del(trigon_core_vkl_device(), &simple_shader.pipeline);

    }
}

void shader_simple_new() {
    shader_simple_del();

    vkl_shader_t shader = { 0 };

    {
        path_os_t path = { 0 };
        path_os_t vert_path = { 0 };
        path_os_t frag_path = { 0 };

        path_get_current(&path);
        path_change_dir(&path, "data", &path);
        path_find_file(&path, "base_vert.spv", &vert_path);
        path_find_file(&path, "base_frag.spv", &frag_path);

        vkl_shader_new(trigon_core_vkl_device(), &shader, vert_path.data, frag_path.data);
    }

    vkl_descriptor_t descriptors[MAX_DESC_PER_SET] = { 0 };
    vkl_descriptor_new(0, 1, SHADER_STATIC_BUFFER, SHADER_STAGE_VERTEX, &descriptors[0]);

    vkl_descriptor_set_t* global_set = (vkl_descriptor_set_t*)shader_global_data_ptr();

    vkl_descriptor_set_new(
        trigon_core_vkl_device(),
        descriptors,
        1,
        &simple_shader.sets
    );

    vkl_pipeline_refs_t references = {
       .device = trigon_core_vkl_device(),
       .swapchain = trigon_core_swapchain(),
       .shader = &shader,
       .sets = {
           global_set,
           &simple_shader.sets
       },
       .sets_count = 2
    };

	vkl_pipeline_config_t config = { 0 };
	vkl_pipeline_config_set_default(&config);
	vkl_pipeline_new(&simple_shader.pipeline, &references, &config);
	vkl_shader_del(trigon_core_vkl_device(), &shader);

    vkl_buffer_info_t matrix_info = {
       .count = 1,
       .device = trigon_core_vkl_device(),
       .stride = sizeof(shader_simple_data_t),
       .usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
    };
    vkl_buffer_new(&matrix_info, &simple_shader.matrix_buffer);

    vkl_buffer_set(&simple_shader.matrix_buffer, &simple_shader.matrices);

    vkl_buffer_t* buffers[] = { &simple_shader.matrix_buffer };
    vkl_descriptor_set_update_ubo(
        trigon_core_vkl_device(),
        &simple_shader.sets,
        0,
        buffers,
        1
    );

}

void shader_simple_draw(uint32_t mesh) {
    if (simple_shader.matrix_count <= 0) return;

    vkl_pipeline_bind(
        trigon_core_vkl_state(),
        &simple_shader.pipeline
    );

    vkl_descriptor_set_bind(trigon_core_vkl_state(), &simple_shader.pipeline, shader_global_data_ptr(), 0);
    vkl_descriptor_set_bind(trigon_core_vkl_state(), &simple_shader.pipeline, &simple_shader.sets, 1);
    mesh_bind(mesh);

    mesh_draw(mesh, simple_shader.matrix_count);
}

uint32_t shader_simple_new_instance() {
    uint32_t id = simple_shader.matrix_count;
    simple_shader.matrix_count++;
    return id;
}

void shader_simple_set_instance(uint32_t id, mat4 data) {
    glm_mat4_copy(data, &simple_shader.matrices[id]);
}

void shader_simple_submit_instances() {
    vkl_buffer_set(&simple_shader.matrix_buffer, &simple_shader.matrices);
}
