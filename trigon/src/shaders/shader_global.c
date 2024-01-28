#include "shader_global.h"
#include <vkl/vkl.h>
#include <core/core.h>
#include <memory.h>

static shader_global_data_t	global_data		= { 0 };
static vkl_descriptor_set_t global_set		= { 0 };
static vkl_buffer_t			global_buffer	= { 0 };

void shader_global_data_get(shader_global_data_t* out) {
    if (out == NULL) {
        printf("shader_global_data_get(out): out was NULL!\n");
        exit(1);
    }

    memcpy(out, &global_data, sizeof(shader_global_data_t));
}
void shader_global_data_set(shader_global_data_t* in) {
    
    if (in == NULL) {
        printf("shader_global_data_set(out): out was NULL!\n");
        exit(1);
    }

    memcpy(&global_data, in, sizeof(shader_global_data_t));
	if (!global_set.initialized) {

        vkl_descriptor_t descriptors[MAX_DESC_PER_SET] = { 0 };
        vkl_descriptor_new(0, 1, SHADER_STATIC_BUFFER, SHADER_STAGE_VERTEX, &descriptors[0]);

        vkl_descriptor_set_new(
            trigon_core_vkl_device(),
            descriptors,
            1,
            &global_set
        );

        vkl_buffer_info_t buffer_info = {
           .count = 1,
           .device = trigon_core_vkl_device(),
           .stride = sizeof(shader_global_data_t),
           .usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
        };
        vkl_buffer_new(&buffer_info, &global_buffer);

        vkl_buffer_set(&global_buffer, &global_data);

        vkl_buffer_t* buffers[] = { &global_buffer };
        vkl_descriptor_set_update_ubo(
            trigon_core_vkl_device(),
            &global_set,
            0,
            buffers,
            1
        );
        return;
	}

    vkl_buffer_set(&global_buffer, &global_data);
}

void shader_global_data_del() {
    vkl_buffer_del(&global_buffer);
    vkl_descriptor_set_del(trigon_core_vkl_device(), &global_set);
    memset(&global_data, 0, sizeof(shader_global_data_t));
}
void* shader_global_data_ptr() {
    if (!global_set.initialized) {
        printf("global data is not set!\n");
        exit(1);
    }
    return &global_set;
}
