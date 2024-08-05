#include "trigon.h"
#include <stdio.h>
#include "trigon/core/dev/vk/vk.h"








i32 trigon_main() {
    app_info_t info = app_main();
    vkctx_new(info.app_name, info.app_version);

    while (!vkctx_get()->win_closing) {
        vkctx_poll();

        vkctx_frame_begin();

        vkctx_frame_end();
    }

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