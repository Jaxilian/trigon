#include "trigon/trigon.h"
#include "trigon/scene/scene.h"

int local_main() {

    app_info_t info = {
        .name = "Trigon Editor",
        .services = 0, // declared later
        .version = {
            .version    = 1,
            .major      = 0,
            .minor      = 0,
            .patch      = 0
        },
    };

    FLAG_ON(info.services, SERVICE_RENDERER);

    scene_load("my_scene");

    trigon_start(&info);
    return 0;
}

#if defined(_WIN32) && defined(_NDEBUG)
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
int WINAPI wWinMain(HINSTANCE hi, HINSTANCE hp, PWSTR cmd, int show) {
    return local_main();
}
#else
int main() {
    return local_main();
}
#endif
