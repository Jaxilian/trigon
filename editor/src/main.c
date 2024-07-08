#include "trigon/app.h"
#include "trigon/core/math/mathr.h"
#include "trigon/shaders/shader.h"
#include "trigon/core/vfs/vfs.h"
#include "trigon/types/texture.h"
uint32_t test_shader = 0;

void engine_start() {
    shader_info_t info = {0};

    test_shader = shader_new(&info);

    mathr_unit_test();

    vpath_t texture_path = { 0 };
    vpath_t path = { 0 };
    vfs_current_path(path);
    if (!vfs_find(path, texture_path, "texture.jpg", true)) {
        return;
    }

    texture_info_t tinfo = texture_read(texture_path);
    uint32_t texture_id = texture_new(&tinfo);
}

void engine_update() {

}

void engine_quit() {

}

int app_main() {
    vpath_t current = { 0 };
    vfs_current_path(current);
    debug_log("%s\n", current);
    app_info_t info = {
        .version = {1,0,0,0},
        .name = "editor",
        .on_start = engine_start,
        .on_update = engine_update,
        .on_quit = engine_quit
    };

    app_load(&info);
    return 0;
}

#ifdef _WIN32
#ifdef _DEBUG
int main() {
    return app_main();
}
#else
#include <windows.h>
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    return app_main();
}
#endif
#else
int main() {
    return app_main();
}
#endif
