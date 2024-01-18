#include "gui.h"
#include <vkl/vkl.h>
#include <vstd/vfs.h>
#include <stdio.h>
#include <cglm/cglm.h>
#include <string.h>

static vkl_pipeline_t pipeline = { 0 };


static void gui_pipeline_new(vkl_device_t* dev) {

    path_os_t path      = { 0 };
    path_os_t vert_path = { 0 };
    path_os_t frag_path = { 0 };

    path_get_current(&path);
    path_change_dir(&path, "data", &path);
    path_find_file(&path, "test_vert.spv", &vert_path);
    path_find_file(&path, "test_frag.spv", &frag_path);

    vkl_shader_t shader = { 0 };
    vkl_shader_new(dev, &shader, vert_path.data, frag_path.data);
    printf("Compiled shader in %s:%d\n", __FILE__, __LINE__);


    //vkl_pipeline_new(&pipeline)

    vkl_shader_del(dev, &shader);
}

bool gui_new(vkl_device_t* dev) {
    gui_pipeline_new(dev);
    return true;
}

bool gui_draw_quad() {
    return true;
}