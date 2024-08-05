#include "trigon/trigon.h"
#include <stdio.h>

#include "trigon/renderer/renderer.h"
#include "trigon/core/dev/win/window.h"

trigon_t::trigon_t() {
   
   
}  

trigon_t::~trigon_t() {

}

void trigon_t::quit() {
    running = false;
}

int trigon_t::__trigon_main() {
    app_info_t app = app_main();

    cassert(
        strlen(app.name) >= 3,
        "app_info_t.name needs to be at least 3 chars\n"
    );

    renderer_t rend(app.name, app.version);
    window_t::main().set_name(app.name);


    app.start_cb();

    while (trigon_t::running) {
        window_t::poll_events();
        if (!window_t::main().active()) {
            quit();
            continue;
        }
        
        app.update_cb();
    
        if (rend.get_cmdr().frame_begin() != VK_SUCCESS) continue;


        rend.get_cmdr().frame_end();
       
    }
    vkDeviceWaitIdle(rend.get_device().device);
  
    rend.get_swap().destroy();
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