#include "trigon/trigon.h"
#include <stdio.h>

#include "trigon/core/dev/gpu/gpu.h"
#include "trigon/core/dev/win/window.h"

trigon_t::trigon_t() {
    app_info_t app = app_main();
    cassert(
        strlen(app.name) >= 3,
        "app_info_t.name needs to be at least 3 chars\n"
    );
       
  
    vkinst_t& instance = vkinst_t::ref();
    instance.load(app.name, app.version);

    window_t&   window  = window_t::main();
    gpu_t&      gpu     = gpu_t::ref();
    vgpu_t&     vgpu    = vgpu_t::ref();
    swap_t      swap(window);
    window.swap = &swap;

    vkDeviceWaitIdle(vgpu.handle);
}  

trigon_t::~trigon_t() {

}

void trigon_t::quit() {
    running = false;
}

int trigon_t::__trigon_main() {
 

    return 0;
}

#if defined(_WIN32) && defined(_NDEBUG)
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    trigon_t& trigon = trigon_t::ref();
    return trigon.__trigon_main();
}
#else
int main() {
    trigon_t& trigon = trigon_t::ref();
    return trigon.__trigon_main();
}
#endif