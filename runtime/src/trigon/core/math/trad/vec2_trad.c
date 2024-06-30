#include "mathtrad.h"
#include <math.h>
#include <stdio.h>

void _tradvec2_add(vec2_t r, const vec2_t a, const vec2_t b) {
    for (int i = 0; i < 2; i++) {
        r[i] = a[i] + b[i];
    }
}

void _tradvec2_sub(vec2_t r, const vec2_t a, const vec2_t b) {
    for (int i = 0; i < 2; i++) {
        r[i] = a[i] - b[i];
    }
}

void _tradvec2_mul(vec2_t r, const vec2_t a, const vec2_t b) {
    for (int i = 0; i < 2; i++) {
        r[i] = a[i] * b[i];
    }
}

void _tradvec2_div(vec2_t r, const vec2_t a, const vec2_t b) {
    for (int i = 0; i < 2; i++) {
        r[i] = a[i] / b[i];
    }
}

void _tradvec2_neg(vec2_t r, const vec2_t v) {
    for (int i = 0; i < 2; i++) {
        r[i] = -v[i];
    }
}

float _tradvec2_dot(const vec2_t a, const vec2_t b) {
    float result = 0.0f;
    for (int i = 0; i < 2; i++) {
        result += a[i] * b[i];
    }
    return result;
}

float _tradvec2_magnitude(const vec2_t v) {
    return sqrtf(_tradvec2_dot(v, v));
}

void _tradvec2_normalize(vec2_t r, const vec2_t v) {
    float magnitude = _tradvec2_magnitude(v);
    for (int i = 0; i < 2; i++) {
        r[i] = v[i] / magnitude;
    }
}

void _tradvec2_cross(vec2_t r, const vec2_t a, const vec2_t b) {
    float cross_product = a[0] * b[1] - a[1] * b[0];
    r[0] = cross_product;
    r[1] = 0.0f;
}

void _tradvec2_project(vec2_t r, const vec2_t a, const vec2_t b) {
    float scalar = _tradvec2_dot(a, b) / _tradvec2_dot(b, b);
    for (int i = 0; i < 2; i++) {
        r[i] = b[i] * scalar;
    }
}

float _tradvec2_distance(const vec2_t a, const vec2_t b) {
    vec2_t diff;
    _tradvec2_sub(diff, a, b);
    return _tradvec2_magnitude(diff);
}

void _tradvec2_lerp(vec2_t r, const vec2_t a, const vec2_t b, float t) {
    for (int i = 0; i < 2; i++) {
        r[i] = a[i] * (1.0f - t) + b[i] * t;
    }
}

void _tradvec2_copy(vec2_t r, const vec2_t v) {
    for (int i = 0; i < 2; i++) {
        r[i] = v[i];
    }
}

bool _tradvec2_eq(const vec2_t a, const vec2_t b) {
    for (int i = 0; i < 2; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

void _tradvec2_scalar_mul(vec2_t r, const vec2_t v, float s) {
    for (int i = 0; i < 2; i++) {
        r[i] = v[i] * s;
    }
}

void _tradvec2_scalar_div(vec2_t r, const vec2_t v, float s) {
    for (int i = 0; i < 2; i++) {
        r[i] = v[i] / s;
    }
}

void _tradvec2_reflect(vec2_t r, const vec2_t v, const vec2_t normal) {
    float scalar = 2.0f * _tradvec2_dot(v, normal);
    vec2_t temp;
    _tradvec2_scalar_mul(temp, normal, scalar);
    _tradvec2_sub(r, v, temp);
}

float _tradvec2_angle_between(const vec2_t a, const vec2_t b) {
    return acosf(_tradvec2_dot(a, b) / (_tradvec2_magnitude(a) * _tradvec2_magnitude(b)));
}

void _tradvec2_hadamard(vec2_t r, const vec2_t a, const vec2_t b) {
    _tradvec2_mul(r, a, b);
}

void _tradvec2_min(vec2_t r, const vec2_t a, const vec2_t b) {
    for (int i = 0; i < 2; i++) {
        r[i] = fminf(a[i], b[i]);
    }
}

void _tradvec2_max(vec2_t r, const vec2_t a, const vec2_t b) {
    for (int i = 0; i < 2; i++) {
        r[i] = fmaxf(a[i], b[i]);
    }
}

void _tradvec2_clamp(vec2_t r, const vec2_t v, const vec2_t min, const vec2_t max) {
    for (int i = 0; i < 2; i++) {
        r[i] = fminf(fmaxf(v[i], min[i]), max[i]);
    }
}

void _tradvec2_abs(vec2_t r, const vec2_t v) {
    for (int i = 0; i < 2; i++) {
        r[i] = fabsf(v[i]);
    }
}

void _tradvec2_sqrt(vec2_t r, const vec2_t v) {
    for (int i = 0; i < 2; i++) {
        r[i] = sqrtf(v[i]);
    }
}

float _tradvec2_distance_squared(const vec2_t a, const vec2_t b) {
    vec2_t diff;
    _tradvec2_sub(diff, a, b);
    return _tradvec2_dot(diff, diff);
}

void _tradvec2_test() {
    vec2_t a = { 1.0f, 2.0f };
    vec2_t b = { 4.0f, 3.0f };
    vec2_t r;
    float scalar = 2.0f;

    _tradvec2_add(r, a, b);
    print_vec2("a + b", r);

    _tradvec2_sub(r, a, b);
    print_vec2("a - b", r);

    _tradvec2_mul(r, a, b);
    print_vec2("a * b (element-wise)", r);

    _tradvec2_div(r, a, b);
    print_vec2("a / b (element-wise)", r);

    _tradvec2_neg(r, a);
    print_vec2("-a", r);

    float dot_product = _tradvec2_dot(a, b);
    printf("a . b: %f\n", dot_product);

    float magnitude = _tradvec2_magnitude(a);
    printf("|a|: %f\n", magnitude);

    _tradvec2_normalize(r, a);
    print_vec2("normalize(a)", r);

    _tradvec2_cross(r, a, b);
    print_vec2("a x b", r);

    _tradvec2_project(r, a, b);
    print_vec2("project(a, b)", r);

    float distance = _tradvec2_distance(a, b);
    printf("distance(a, b): %f\n", distance);

    _tradvec2_lerp(r, a, b, 0.5f);
    print_vec2("lerp(a, b, 0.5)", r);

    _tradvec2_copy(r, a);
    print_vec2("copy(a)", r);

    bool equal = _tradvec2_eq(a, b);
    printf("a == b: %s\n", equal ? "true" : "false");

    _tradvec2_scalar_mul(r, a, scalar);
    print_vec2("a * scalar", r);

    _tradvec2_scalar_div(r, a, scalar);
    print_vec2("a / scalar", r);

    _tradvec2_reflect(r, a, b);
    print_vec2("reflect(a, b)", r);

    float angle = _tradvec2_angle_between(a, b);
    printf("angle_between(a, b): %f radians\n", angle);

    _tradvec2_hadamard(r, a, b);
    print_vec2("hadamard(a, b)", r);

    _tradvec2_min(r, a, b);
    print_vec2("min(a, b)", r);

    _tradvec2_max(r, a, b);
    print_vec2("max(a, b)", r);

    _tradvec2_clamp(r, a, b, (vec2_t) { 0.0f, 0.0f });
    print_vec2("clamp(a, b, [0, 0])", r);

    _tradvec2_abs(r, a);
    print_vec2("abs(a)", r);

    _tradvec2_sqrt(r, a);
    print_vec2("sqrt(a)", r);

    float dist_squared = _tradvec2_distance_squared(a, b);
    printf("distance_squared(a, b): %f\n", dist_squared);
}