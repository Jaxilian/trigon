#include "mathr.h"
#include <stdio.h>

#include "trad/mathtrad.h"

void print_vec4(const char* label, const vec4_t v) {
    printf("%s: [%f, %f, %f, %f]\n", label, v[0], v[1], v[2], v[3]);
}

void print_vec3(const char* label, const vec3_t v) {
    printf("%s: [%f, %f, %f]\n", label, v[0], v[1], v[2]);
}

void print_vec2(const char* label, const vec2_t v) {
    printf("%s: [%f, %f]\n", label, v[0], v[1]);
}


void mathr_unit_test() {
    //_tradvec4_test();
    //_tradvec3_test();
    //_tradvec2_test();
}