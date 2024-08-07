#include "trigon.h"
#include <stdio.h>
#include "trigon/core/dev/vk/vk.h"
#include "trigon/shaders/shader.h"

i32 trigon_main() {
    app_info_t info = app_main();
    vkctx_new(info.app_name, info.app_version);
    __sys_shader_init();



    info.app_start_cb();

    while (!vkctx_get()->win_closing) {
        vkctx_poll();
        info.app_update_cb();

        vkctx_frame_begin();

        vkctx_frame_end();
    }
    info.app_quit_cb();
    vkctx_del();

    return 0;
}


#if defined(_WIN32) && defined(_NDEBUG)
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
int WINAPI WinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    return trigon_main();
}

#else
int main() {
    return trigon_main();
}
#endif