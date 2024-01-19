#include "gui.h"
#include "core/core.h"
#include <vkl/vkl.h>
#include <vstd/vfs.h>
#include <stdio.h>
#include <cglm/cglm.h>
#include <string.h>
#include "types/mesh.h"
#include "shaders/shader_global.h"

static vkl_pipeline_t pipeline = { 0 };
static uint32_t quad_mesh = 0;
static vkl_descriptor_set_t sets = { 0 };
static vkl_buffer_t matrix_buffer = { 0 };

#define gui_vertex_binding_count 1
#define gui_vertex_attrb_count 2

typedef struct {
    mat4 matrix[128];
    vec3 colors[128];
} gui_data_t;

static uint32_t quads_in_use = 0;
static gui_data_t quad_matrices = { 0 };

static void gui_pipeline_new() {
    path_os_t path      = { 0 };
    path_os_t vert_path = { 0 };
    path_os_t frag_path = { 0 };

    path_get_current(&path);
    path_change_dir(&path, "data", &path);
    path_find_file(&path, "test_vert.spv", &vert_path);
    path_find_file(&path, "test_frag.spv", &frag_path);

    printf("size of gui %zu\n", sizeof(gui_data_t));

    vkl_shader_t shader = { 0 };
    vkl_shader_new(trigon_core_vkl_device(), &shader, vert_path.data, frag_path.data);
    
    vkl_descriptor_t descriptors[MAX_DESC_PER_SET] = { 0 };
    vkl_descriptor_new(0, 1, SHADER_STATIC_BUFFER, SHADER_STAGE_VERTEX, &descriptors[0]);

    vkl_descriptor_set_new(
        trigon_core_vkl_device(),
        descriptors,
        1,
        &sets
    );

    vkl_descriptor_set_t* global_set = (vkl_descriptor_set_t*)shader_global_data_ptr();

    vkl_pipeline_refs_t references = {
        .device = trigon_core_vkl_device(),
        .swapchain = trigon_core_swapchain(),
        .shader = &shader,
        .sets = {
            global_set,
            &sets
        },
        .sets_count = 2
    };

    VkVertexInputBindingDescription gui_vertex_binding[gui_vertex_binding_count] = {
        {
            .binding = 0,
            .stride = sizeof(gui_vertex_t),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        }
    };

    VkVertexInputAttributeDescription gui_vertex_attributes[gui_vertex_attrb_count] = {
    {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(gui_vertex_t, pos)
    },
    {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(gui_vertex_t, uv)
    }
    };

    vkl_pipeline_config_t config = { 0 };
    vkl_pipeline_config_set_default(&config);

    config.vertex_attribute = gui_vertex_attributes;
    config.vertex_attribute_count = gui_vertex_attrb_count;
    config.vertex_binding = gui_vertex_binding;
    config.vertex_binding_count = gui_vertex_binding_count;

    config.depth_stencil.depthTestEnable = VK_FALSE;
    config.depth_stencil.depthWriteEnable = VK_FALSE;
    config.rasterization.cullMode = VK_CULL_MODE_NONE;
    config.multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    config.color_blend_attachment.blendEnable = VK_TRUE;
    config.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    config.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    config.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    config.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    config.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    config.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    vkl_pipeline_new(&pipeline, &references, &config);
    vkl_shader_del(trigon_core_vkl_device(), &shader);

    vkl_buffer_info_t matrix_info = {
        .count = 1,
        .device = trigon_core_vkl_device(),
        .stride = sizeof(gui_data_t),
        .usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
    };
    vkl_buffer_new(&matrix_info, &matrix_buffer);

    vkl_buffer_set(&matrix_buffer, &quad_matrices);

    vkl_buffer_t* buffers[] = { &matrix_buffer };
    vkl_descriptor_set_update_ubo(
        trigon_core_vkl_device(),
        &sets,
        0,
        buffers,
        1
    );
}

#define vp 1.00f
static void gui_mesh_new() {
    gui_vertex_t gui_vertices[] = {
       {{-vp, -vp}, {0.0f, 0.0f}}, // Bottom left
       {{ vp, -vp}, {1.0f, 0.0f}}, // Bottom right
       {{ vp,  vp}, {1.0f, 1.0f}}, // Top right
       {{-vp,  vp}, {0.0f, 1.0f}}  // Top left
    };

    uint32_t gui_indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    mesh_info_t info = {
        .vertex_stride = sizeof(gui_vertex_t),
        .vertex_buffer = gui_vertices,
        .vertex_count = 4,
        .index_count = 6,
        .indices = gui_indices
    };

    quad_mesh = mesh_new(&info);
}

void gui_rebuild() {
    vkl_descriptor_set_del(trigon_core_vkl_device(), &sets);
    vkl_pipeline_del(trigon_core_vkl_device(), &pipeline);
    gui_pipeline_new(trigon_core_vkl_device());
}

bool gui_new() {
    gui_pipeline_new(trigon_core_vkl_device());
    gui_mesh_new();
   
    signal_connect(ON_WINDOW_RESIZED_SIGNAL, gui_rebuild);

    return true;
}

void gui_pipeline_del() {
    mesh_del(quad_mesh);
    vkl_buffer_del(&matrix_buffer);
    vkl_descriptor_set_del(trigon_core_vkl_device(), &sets);
    vkl_pipeline_del(trigon_core_vkl_device(), &pipeline);
}

void gui_draw() {
    if (quads_in_use <= 0) return;

    vkl_pipeline_bind(
        trigon_core_vkl_state(),
        &pipeline
    );

    vkl_descriptor_set_bind(trigon_core_vkl_state(), &pipeline, shader_global_data_ptr(),0);
    vkl_descriptor_set_bind(trigon_core_vkl_state(), &pipeline, &sets,1);
    mesh_bind(quad_mesh);
    
    mesh_draw(quad_mesh, quads_in_use);
}

void gui_add(gui_quad_t* data) {
    glm_mat4_copy(data->matrix, quad_matrices.matrix[quads_in_use]);
    glm_vec3_copy(data->colors, quad_matrices.colors[quads_in_use]);
    quads_in_use++;
}

void gui_submit() {
    vkl_buffer_set(&matrix_buffer, &quad_matrices);
}

bool gui_draw_quad() {
    return true;
}

