#include "mathtrad.h"
#include "../mathr.h"
#include <math.h>
#include <stdio.h>

void _tradvec3_add(vec3_t r, const vec3_t a, const vec3_t b) {
    for (int i = 0; i < 3; i++) {
        r[i] = a[i] + b[i];
    }
}

void _tradvec3_sub(vec3_t r, const vec3_t a, const vec3_t b) {
    for (int i = 0; i < 3; i++) {
        r[i] = a[i] - b[i];
    }
}

void _tradvec3_mul(vec3_t r, const vec3_t a, const vec3_t b) {
    for (int i = 0; i < 3; i++) {
        r[i] = a[i] * b[i];
    }
}

void _tradvec3_div(vec3_t r, const vec3_t a, const vec3_t b) {
    for (int i = 0; i < 3; i++) {
        r[i] = a[i] / b[i];
    }
}

void _tradvec3_neg(vec3_t r, const vec3_t v) {
    for (int i = 0; i < 3; i++) {
        r[i] = -v[i];
    }
}

float _tradvec3_dot(const vec3_t a, const vec3_t b) {
    float result = 0.0f;
    for (int i = 0; i < 3; i++) {
        result += a[i] * b[i];
    }
    return result;
}

float _tradvec3_magnitude(const vec3_t v) {
    return sqrtf(_tradvec3_dot(v, v));
}

void _tradvec3_normalize(vec3_t r, const vec3_t v) {
    float magnitude = _tradvec3_magnitude(v);
    for (int i = 0; i < 3; i++) {
        r[i] = v[i] / magnitude;
    }
}

void _tradvec3_cross(vec3_t r, const vec3_t a, const vec3_t b) {
    r[0] = a[1] * b[2] - a[2] * b[1];
    r[1] = a[2] * b[0] - a[0] * b[2];
    r[2] = a[0] * b[1] - a[1] * b[0];
}

void _tradvec3_project(vec3_t r, const vec3_t a, const vec3_t b) {
    float scalar = _tradvec3_dot(a, b) / _tradvec3_dot(b, b);
    for (int i = 0; i < 3; i++) {
        r[i] = b[i] * scalar;
    }
}

float _tradvec3_distance(const vec3_t a, const vec3_t b) {
    vec3_t diff;
    _tradvec3_sub(diff, a, b);
    return _tradvec3_magnitude(diff);
}

void _tradvec3_lerp(vec3_t r, const vec3_t a, const vec3_t b, float t) {
    for (int i = 0; i < 3; i++) {
        r[i] = a[i] * (1.0f - t) + b[i] * t;
    }
}

void _tradvec3_copy(vec3_t r, const vec3_t v) {
    for (int i = 0; i < 3; i++) {
        r[i] = v[i];
    }
}

bool _tradvec3_eq(const vec3_t a, const vec3_t b) {
    for (int i = 0; i < 3; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

void _tradvec3_scalar_mul(vec3_t r, const vec3_t v, float s) {
    for (int i = 0; i < 3; i++) {
        r[i] = v[i] * s;
    }
}

void _tradvec3_scalar_div(vec3_t r, const vec3_t v, float s) {
    for (int i = 0; i < 3; i++) {
        r[i] = v[i] / s;
    }
}

void _tradvec3_reflect(vec3_t r, const vec3_t v, const vec3_t normal) {
    float scalar = 2.0f * _tradvec3_dot(v, normal);
    vec3_t temp;
    _tradvec3_scalar_mul(temp, normal, scalar);
    _tradvec3_sub(r, v, temp);
}

float _tradvec3_angle_between(const vec3_t a, const vec3_t b) {
    return acosf(_tradvec3_dot(a, b) / (_tradvec3_magnitude(a) * _tradvec3_magnitude(b)));
}

void _tradvec3_hadamard(vec3_t r, const vec3_t a, const vec3_t b) {
    _tradvec3_mul(r, a, b);
}

void _tradvec3_min(vec3_t r, const vec3_t a, const vec3_t b) {
    for (int i = 0; i < 3; i++) {
        r[i] = fminf(a[i], b[i]);
    }
}

void _tradvec3_max(vec3_t r, const vec3_t a, const vec3_t b) {
    for (int i = 0; i < 3; i++) {
        r[i] = fmaxf(a[i], b[i]);
    }
}

void _tradvec3_clamp(vec3_t r, const vec3_t v, const vec3_t min, const vec3_t max) {
    for (int i = 0; i < 3; i++) {
        r[i] = fminf(fmaxf(v[i], min[i]), max[i]);
    }
}

void _tradvec3_abs(vec3_t r, const vec3_t v) {
    for (int i = 0; i < 3; i++) {
        r[i] = fabsf(v[i]);
    }
}

void _tradvec3_sqrt(vec3_t r, const vec3_t v) {
    for (int i = 0; i < 3; i++) {
        r[i] = sqrtf(v[i]);
    }
}

float _tradvec3_distance_squared(const vec3_t a, const vec3_t b) {
    vec3_t diff;
    _tradvec3_sub(diff, a, b);
    return _tradvec3_dot(diff, diff);
}

void _tradvec3_rotate(vec3_t r, const vec3_t v, const vec3_t axis, float angle) {
    float cos_theta = cosf(angle);
    float sin_theta = sinf(angle);
    float dot_va = _tradvec3_dot(v, axis);
    vec3_t temp, cross_product;

    for (int i = 0; i < 3; i++) {
        temp[i] = axis[i] * dot_va * (1 - cos_theta);
    }

    cross_product[0] = axis[1] * v[2] - axis[2] * v[1];
    cross_product[1] = axis[2] * v[0] - axis[0] * v[2];
    cross_product[2] = axis[0] * v[1] - axis[1] * v[0];

    for (int i = 0; i < 3; i++) {
        r[i] = v[i] * cos_theta + cross_product[i] * sin_theta + temp[i];
    }
}

void _tradvec3_test() {
    vec3_t a = { 1.0f, 2.0f, 3.0f };
    vec3_t b = { 4.0f, 3.0f, 2.0f };
    vec3_t r;
    float scalar = 2.0f;

    _tradvec3_add(r, a, b);
    print_vec3("a + b", r);

    _tradvec3_sub(r, a, b);
    print_vec3("a - b", r);

    _tradvec3_mul(r, a, b);
    print_vec3("a * b (element-wise)", r);

    _tradvec3_div(r, a, b);
    print_vec3("a / b (element-wise)", r);

    _tradvec3_neg(r, a);
    print_vec3("-a", r);

    float dot_product = _tradvec3_dot(a, b);
    printf("a . b: %f\n", dot_product);

    float magnitude = _tradvec3_magnitude(a);
    printf("|a|: %f\n", magnitude);

    _tradvec3_normalize(r, a);
    print_vec3("normalize(a)", r);

    _tradvec3_cross(r, a, b);
    print_vec3("a x b", r);

    _tradvec3_project(r, a, b);
    print_vec3("project(a, b)", r);

    float distance = _tradvec3_distance(a, b);
    printf("distance(a, b): %f\n", distance);

    _tradvec3_lerp(r, a, b, 0.5f);
    print_vec3("lerp(a, b, 0.5)", r);

    _tradvec3_copy(r, a);
    print_vec3("copy(a)", r);

    bool equal = _tradvec3_eq(a, b);
    printf("a == b: %s\n", equal ? "true" : "false");

    _tradvec3_scalar_mul(r, a, scalar);
    print_vec3("a * scalar", r);

    _tradvec3_scalar_div(r, a, scalar);
    print_vec3("a / scalar", r);

    _tradvec3_reflect(r, a, b);
    print_vec3("reflect(a, b)", r);

    float angle = _tradvec3_angle_between(a, b);
    printf("angle_between(a, b): %f radians\n", angle);

    _tradvec3_hadamard(r, a, b);
    print_vec3("hadamard(a, b)", r);

    _tradvec3_min(r, a, b);
    print_vec3("min(a, b)", r);

    _tradvec3_max(r, a, b);
    print_vec3("max(a, b)", r);

    _tradvec3_clamp(r, a, b, (vec3_t) { 0.0f, 0.0f, 0.0f });
    print_vec3("clamp(a, b, [0, 0, 0])", r);

    _tradvec3_abs(r, a);
    print_vec3("abs(a)", r);

    _tradvec3_sqrt(r, a);
    print_vec3("sqrt(a)", r);

    float dist_squared = _tradvec3_distance_squared(a, b);
    printf("distance_squared(a, b): %f\n", dist_squared);

    _tradvec3_rotate(r, a, (vec3_t) { 0.0f, 1.0f, 0.0f }, (float)(M_PI / 4.0));
    print_vec3("rotate(a, [0, 1, 0], PI/4)", r);
}