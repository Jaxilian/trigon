#include "trigon/app.h"
#include "trigon/core/math/tgmath.h"

#define printf debug_log

void print_vec3(const char* label, const vec3_t v) {
    printf("%s: (%f, %f, %f)\n", label, v[0], v[1], v[2]);
}


void print_bool(const char* label, bool b) {
    printf("%s: %s\n", label, b ? "true" : "false");
}

void engine_start() {
    vec3_t a = { 1.0f, 2.0f, 3.0f };
    vec3_t b = { 4.0f, 5.0f, 6.0f };
    vec3_t result;
    vec3_t min_vec = { 0.5f, 1.5f, 2.5f };
    vec3_t max_vec = { 1.5f, 2.5f, 3.5f };

    _avxvec3_add(result, a, b);
    print_vec3("Addition", result);

    _avxvec3_sub(result, a, b);
    print_vec3("Subtraction", result);

    _avxvec3_mul(result, a, b);
    print_vec3("Multiplication", result);

    _avxvec3_div(result, a, b);
    print_vec3("Division", result);

    _avxvec3_neg(result, a);
    print_vec3("Negation", result);

    float dot_product = _avxvec3_dot(a, b);
    printf("Dot Product: %f\n", dot_product);

    float magnitude = _avxvec3_magnitude(a);
    printf("Magnitude: %f\n", magnitude);

    _avxvec3_normalize(result, a);
    print_vec3("Normalization", result);

    _avxvec3_cross(result, a, b);
    print_vec3("Cross Product", result);

    _avxvec3_project(result, a, b);
    print_vec3("Projection", result);

    float distance = _avxvec3_distance(a, b);
    printf("Distance: %f\n", distance);

    _avxvec3_lerp(result, a, b, 0.5f);
    print_vec3("Lerp", result);

    _avxvec3_copy(result, a);
    print_vec3("Copy", result);

    bool is_equal = _avxvec3_eq(a, b);
    print_bool("Equality", is_equal);

    _avxvec3_scalar_mul(result, a, 2.0f);
    print_vec3("Scalar Multiplication", result);

    _avxvec3_scalar_div(result, a, 2.0f);
    print_vec3("Scalar Division", result);

    vec3_t normal = { 0.0f, 1.0f, 0.0f };
    _avxvec3_reflect(result, a, normal);
    print_vec3("Reflect", result);

    float angle = _avxvec3_angle_between(a, b);
    printf("Angle Between: %f\n", angle);

    _avxvec3_hadamard(result, a, b);
    print_vec3("Hadamard Product", result);

    _avxvec3_min(result, a, b);
    print_vec3("Min", result);

    _avxvec3_max(result, a, b);
    print_vec3("Max", result);

    _avxvec3_clamp(result, a, min_vec, max_vec);
    print_vec3("Clamp", result);

    _avxvec3_abs(result, a);
    print_vec3("Absolute", result);

    _avxvec3_sqrt(result, a);
    print_vec3("Square Root", result);

    mat4_t m = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        1, 2, 3, 1
    };
    _avxvec3_transform(result, a, m);
    print_vec3("Transform", result);
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