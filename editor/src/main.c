#include "trigon/app.h"
#include "trigon/shaders/shader.h"
#include "trigon/core/vfs/vfs.h"

void engine_start() {
    //shader_new();
    //shader_compile();

    vpath_t path = { 0 };
    vfs_init("C:/projects/", path);
    vfs_create(path, "example.txt", VFS_TYPE_FILE);

}

void engine_update() {


}

void engine_quit() {
    //shader_del();
}


int app_main() {
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
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    return app_main();
}
#endif
#else
int main() {
    return app_main();
}
#endif