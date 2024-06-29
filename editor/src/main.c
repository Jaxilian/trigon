#include "trigon/app.h"
#include "trigon/core/math/tgmath.h"

void engine_start() {
    mat4_t mat1 = mat4_identity;
    mat4_t mat2 = mat4_identity;
    mat4_t mat3 = mat4_identity;



}

void engine_update() {


}

void engine_quit() {
 
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