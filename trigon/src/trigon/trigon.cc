#include "trigon/trigon.h"
#include <stdio.h>
#include "trigon/core/renderer/renderer.h"

trigon_t::trigon_t() {
    app_info_t app = app_main();
    cassert(
        strlen(app.name) >= 3,
        "app_info_t.name needs to be at least 3 chars\n"
    );

    renderer_t& renderer    = renderer_t::ref();
    renderer.APP_NAME       = app.name;
    renderer.APP_VERSION    = app.version;
    renderer.init();

    debug_log("%s : %s\n", renderer.ENGINE_NAME, renderer.ENGINE_VERSION.str());
    debug_log("%s : %s\n", renderer.APP_NAME, renderer.APP_VERSION.str());

    window_t::main().set_name(app.name);
}

trigon_t::~trigon_t() {

}

void trigon_t::quit() {
    running = false;
}

int trigon_t::__trigon_main() {
    while (running) {
        window_t::poll_events();

        bool win_active = window_t::main().active();
        if (!win_active) quit();
    }

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