#include "../mathr.h"
#include "mathtrad.h"
#include <stdio.h>
#include "math.h"

void _tradvec4_add(vec4_t r, const vec4_t a, const vec4_t b) {
    for (int i = 0; i < 4; i++) {
        r[i] = a[i] + b[i];
    }
}

void _tradvec4_sub(vec4_t r, const vec4_t a, const vec4_t b) {
    for (int i = 0; i < 4; i++) {
        r[i] = a[i] - b[i];
    }
}

void _tradvec4_mul(vec4_t r, const vec4_t a, const vec4_t b) {
    for (int i = 0; i < 4; i++) {
        r[i] = a[i] * b[i];
    }
}

void _tradvec4_div(vec4_t r, const vec4_t a, const vec4_t b) {
    for (int i = 0; i < 4; i++) {
        r[i] = a[i] / b[i];
    }
}

void _tradvec4_neg(vec4_t r, const vec4_t v) {
    for (int i = 0; i < 4; i++) {
        r[i] = -v[i];
    }
}

float _tradvec4_dot(const vec4_t a, const vec4_t b) {
    float result = 0.0f;
    for (int i = 0; i < 4; i++) {
        result += a[i] * b[i];
    }
    return result;
}

float _tradvec4_magnitude(const vec4_t v) {
    return sqrtf(_tradvec4_dot(v, v));
}

void _tradvec4_normalize(vec4_t r, const vec4_t v) {
    float magnitude = _tradvec4_magnitude(v);
    for (int i = 0; i < 4; i++) {
        r[i] = v[i] / magnitude;
    }
}

void _tradvec4_cross(vec4_t r, const vec4_t a, const vec4_t b) {
    r[0] = a[1] * b[2] - a[2] * b[1];
    r[1] = a[2] * b[0] - a[0] * b[2];
    r[2] = a[0] * b[1] - a[1] * b[0];
    r[3] = 0.0f; 
}

void _tradvec4_project(vec4_t r, const vec4_t a, const vec4_t b) {
    float scalar = _tradvec4_dot(a, b) / _tradvec4_dot(b, b);
    for (int i = 0; i < 4; i++) {
        r[i] = b[i] * scalar;
    }
}

float _tradvec4_distance(const vec4_t a, const vec4_t b) {
    vec4_t diff;
    _tradvec4_sub(diff, a, b);
    return _tradvec4_magnitude(diff);
}

void _tradvec4_lerp(vec4_t r, const vec4_t a, const vec4_t b, float t) {
    for (int i = 0; i < 4; i++) {
        r[i] = a[i] * (1.0f - t) + b[i] * t;
    }
}

void _tradvec4_copy(vec4_t r, const vec4_t v) {
    for (int i = 0; i < 4; i++) {
        r[i] = v[i];
    }
}

bool _tradvec4_eq(const vec4_t a, const vec4_t b) {
    for (int i = 0; i < 4; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

void _tradvec4_scalar_mul(vec4_t r, const vec4_t v, float s) {
    for (int i = 0; i < 4; i++) {
        r[i] = v[i] * s;
    }
}

void _tradvec4_scalar_div(vec4_t r, const vec4_t v, float s) {
    for (int i = 0; i < 4; i++) {
        r[i] = v[i] / s;
    }
}

void _tradvec4_reflect(vec4_t r, const vec4_t v, const vec4_t normal) {
    float scalar = 2.0f * _tradvec4_dot(v, normal);
    vec4_t temp;
    _tradvec4_scalar_mul(temp, normal, scalar);
    _tradvec4_sub(r, v, temp);
}

float _tradvec4_angle_between(const vec4_t a, const vec4_t b) {
    return acosf(_tradvec4_dot(a, b) / (_tradvec4_magnitude(a) * _tradvec4_magnitude(b)));
}

void _tradvec4_hadamard(vec4_t r, const vec4_t a, const vec4_t b) {
    _tradvec4_mul(r, a, b);
}

void _tradvec4_min(vec4_t r, const vec4_t a, const vec4_t b) {
    for (int i = 0; i < 4; i++) {
        r[i] = fminf(a[i], b[i]);
    }
}

void _tradvec4_max(vec4_t r, const vec4_t a, const vec4_t b) {
    for (int i = 0; i < 4; i++) {
        r[i] = fmaxf(a[i], b[i]);
    }
}

void _tradvec4_clamp(vec4_t r, const vec4_t v, const vec4_t min, const vec4_t max) {
    for (int i = 0; i < 4; i++) {
        r[i] = fminf(fmaxf(v[i], min[i]), max[i]);
    }
}

void _tradvec4_abs(vec4_t r, const vec4_t v) {
    for (int i = 0; i < 4; i++) {
        r[i] = fabsf(v[i]);
    }
}

void _tradvec4_sqrt(vec4_t r, const vec4_t v) {
    for (int i = 0; i < 4; i++) {
        r[i] = sqrtf(v[i]);
    }
}

float _tradvec4_distance_squared(const vec4_t a, const vec4_t b) {
    vec4_t diff;
    _tradvec4_sub(diff, a, b);
    return _tradvec4_dot(diff, diff);
}

void _tradvec4_rotate(vec4_t r, const vec4_t v, const vec4_t axis, float angle) {
    float cos_theta = cosf(angle);
    float sin_theta = sinf(angle);
    float dot_va = _tradvec4_dot(v, axis);
    vec4_t temp, cross_product;

    for (int i = 0; i < 3; i++) {
        temp[i] = axis[i] * dot_va * (1 - cos_theta);
    }

    cross_product[0] = axis[1] * v[2] - axis[2] * v[1];
    cross_product[1] = axis[2] * v[0] - axis[0] * v[2];
    cross_product[2] = axis[0] * v[1] - axis[1] * v[0];
    cross_product[3] = 0.0f;

    for (int i = 0; i < 3; i++) {
        r[i] = v[i] * cos_theta + cross_product[i] * sin_theta + temp[i];
    }
    r[3] = v[3]; 
}

void _tradvec4_test() {
    vec4_t a = { 1.0f, 2.0f, 3.0f, 4.0f };
    vec4_t b = { 4.0f, 3.0f, 2.0f, 1.0f };
    vec4_t r;
    float scalar = 2.0f;

    _tradvec4_add(r, a, b);
    print_vec4("a + b", r);

    _tradvec4_sub(r, a, b);
    print_vec4("a - b", r);

    _tradvec4_mul(r, a, b);
    print_vec4("a * b (element-wise)", r);

    _tradvec4_div(r, a, b);
    print_vec4("a / b (element-wise)", r);

    _tradvec4_neg(r, a);
    print_vec4("-a", r);

    float dot_product = _tradvec4_dot(a, b);
    printf("a . b: %f\n", dot_product);

    float magnitude = _tradvec4_magnitude(a);
    printf("|a|: %f\n", magnitude);

    _tradvec4_normalize(r, a);
    print_vec4("normalize(a)", r);

    _tradvec4_cross(r, a, b);
    print_vec4("a x b", r);

    _tradvec4_project(r, a, b);
    print_vec4("project(a, b)", r);

    float distance = _tradvec4_distance(a, b);
    printf("distance(a, b): %f\n", distance);

    _tradvec4_lerp(r, a, b, 0.5f);
    print_vec4("lerp(a, b, 0.5)", r);

    _tradvec4_copy(r, a);
    print_vec4("copy(a)", r);

    bool equal = _tradvec4_eq(a, b);
    printf("a == b: %s\n", equal ? "true" : "false");

    _tradvec4_scalar_mul(r, a, scalar);
    print_vec4("a * scalar", r);

    _tradvec4_scalar_div(r, a, scalar);
    print_vec4("a / scalar", r);

    _tradvec4_reflect(r, a, b);
    print_vec4("reflect(a, b)", r);

    float angle = _tradvec4_angle_between(a, b);
    printf("angle_between(a, b): %f radians\n", angle);

    _tradvec4_hadamard(r, a, b);
    print_vec4("hadamard(a, b)", r);

    _tradvec4_min(r, a, b);
    print_vec4("min(a, b)", r);

    _tradvec4_max(r, a, b);
    print_vec4("max(a, b)", r);

    _tradvec4_clamp(r, a, b, (vec4_t) { 0.0f, 0.0f, 0.0f, 0.0f });
    print_vec4("clamp(a, b, [0, 0, 0, 0])", r);

    _tradvec4_abs(r, a);
    print_vec4("abs(a)", r);

    _tradvec4_sqrt(r, a);
    print_vec4("sqrt(a)", r);

    float dist_squared = _tradvec4_distance_squared(a, b);
    printf("distance_squared(a, b): %f\n", dist_squared);

    _tradvec4_rotate(r, a, (vec4_t) { 0.0f, 1.0f, 0.0f, 0.0f }, (float)(M_PI / 4.0)); 
    print_vec4("rotate(a, [0, 1, 0, 0], PI/4)", r);
}