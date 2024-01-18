#include "gui.h"
#include "core/core.h"
#include <vkl/vkl.h>
#include <vstd/vfs.h>
#include <stdio.h>
#include <cglm/cglm.h>
#include <string.h>

static vkl_pipeline_t pipeline = { 0 };


static void gui_pipeline_new() {
    path_os_t path      = { 0 };
    path_os_t vert_path = { 0 };
    path_os_t frag_path = { 0 };

    path_get_current(&path);
    path_change_dir(&path, "data", &path);
    path_find_file(&path, "test_vert.spv", &vert_path);
    path_find_file(&path, "test_frag.spv", &frag_path);

    vkl_shader_t shader = { 0 };
    vkl_shader_new(trigon_core_vkl_device(), &shader, vert_path.data, frag_path.data);
    printf("Compiled shader in %s:%d\n", __FILE__, __LINE__);

    vkl_descriptor_set_t sets = { 0 };
    vkl_descriptor_t descriptors[MAX_DESC_PER_SET] = { 0 };

    vkl_descriptor_set_new(
        trigon_core_vkl_device(),
        descriptors,
        0,
        &sets
    );

    vkl_pipeline_refs_t references = {
        .device = trigon_core_vkl_device(),
        .swapchain = trigon_core_swapchain(),
        .shader = &shader,
        .sets = &sets,
        .sets_count = 0
    };

    vkl_pipeline_config_t config = { 0 };
    vkl_pipeline_config_set_default(&config);

    vkl_pipeline_new(&pipeline, &references, &config);

    vkl_shader_del(trigon_core_vkl_device(), &shader);
}

bool gui_new() {
    gui_pipeline_new(trigon_core_vkl_device());
    return true;
}

void gui_del() {
    vkl_pipeline_del(trigon_core_vkl_device(), &pipeline);
}

bool gui_draw_quad() {
    return true;
}

