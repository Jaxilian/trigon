#include "renderer.h"
#include "trigon/core/dev/gpu/gpu.h"
#include "trigon/core/dev/win/window.h"


renderer_t::renderer_t() {
    window_t&   window          = window_t::main();
    gpu_t&      gpu             = gpu_t::ref();
    vgpu_t&     vgpu            = vgpu_t::ref();
    swap_t&     swap            = swap_t::ref();

    create_cmdr();

}

void renderer_t::destroy() {
    if (!vgpu_t::ref().handle) return;

    vkDeviceWaitIdle(vgpu_t::ref().handle);


    destroy_cmdr();

    swap_t::ref().destroy();
    vgpu_t::ref().destroy();
    window_t::main().destroy_surface();
    gpu_t::ref().destroy();
    vkinst_t::ref().destroy();
}

renderer_t::~renderer_t() {
    destroy();
}
