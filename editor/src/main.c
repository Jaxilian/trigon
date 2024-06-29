#include "trigon/app.h"
#include "trigon/core/math/tgmath.h"

#define printf debug_log

void print_vec3(const char* label, const vec3_t v) {
    printf("%s: (%f, %f, %f)\n", label, v[0], v[1], v[2]);
}

void print_float(const char* label, float value) {
    printf("%s: %f\n", label, value);
}

void engine_start() {
    vec3_t a = { 1.0f, 2.0f, 3.0f };
    vec3_t b = { 4.0f, 5.0f, 6.0f };
    vec3_t result;
    vec3_t axis = { 0.0f, 1.0f, 0.0f };
    float angle = PI / 4; // 45 degrees

    // Test _avxvec3_distance_squared
    float dist_sq = _avxvec3_distance_squared(a, b);
    print_float("Distance Squared", dist_sq);

    // Test _avxvec3_rotate
    _avxvec3_rotate(result, a, axis, angle);
    print_vec3("Rotation", result);
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