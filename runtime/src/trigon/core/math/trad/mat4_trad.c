#include "mathtrad.h"
#include "math.h"
#include <stdio.h>

// Function to multiply two 4x4 matrices (column major)
void _tradmat4_mul(mat4_t r, const mat4_t a, const mat4_t b) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            r[i * 4 + j] = 0;
            for (int k = 0; k < 4; k++) {
                r[i * 4 + j] += a[k * 4 + j] * b[i * 4 + k];
            }
        }
    }
}

// Function to set scale in a 4x4 matrix
void _tradmat4_setscale(mat4_t r, const vec3_t s) {
    r[0] = s[0];
    r[1] = 0.0f;
    r[2] = 0.0f;
    r[3] = 0.0f;

    r[4] = 0.0f;
    r[5] = s[1];
    r[6] = 0.0f;
    r[7] = 0.0f;

    r[8] = 0.0f;
    r[9] = 0.0f;
    r[10] = s[2];
    r[11] = 0.0f;

    r[12] = 0.0f;
    r[13] = 0.0f;
    r[14] = 0.0f;
    r[15] = 1.0f;
}

// Function to set position in a 4x4 matrix
void _tradmat4_setpos(mat4_t r, const vec3_t s) {
    r[12] = s[0];
    r[13] = s[1];
    r[14] = s[2];
}

// Function to scale a 4x4 matrix
void _tradmat4_scale(mat4_t r, const vec3_t s) {
    mat4_t scale_mat = {
        s[0], 0.0f, 0.0f, 0.0f,
        0.0f, s[1], 0.0f, 0.0f,
        0.0f, 0.0f, s[2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    mat4_t result;
    _tradmat4_mul(result, r, scale_mat);

    for (int i = 0; i < 16; i++) {
        r[i] = result[i];
    }
}

// Function to set lookat matrix
void _tradmat4_lookat(mat4_t r, const vec3_t eye, const vec3_t center, const vec3_t up) {
    vec3_t f, u, s;

    for (int i = 0; i < 3; i++) {
        f[i] = center[i] - eye[i];
    }
    _tradvec3_normalize(f, f);
    _tradvec3_cross(s, f, up);
    _tradvec3_normalize(s, s);
    _tradvec3_cross(u, s, f);

    mat4_t lookat_mat = {
        s[0], u[0], -f[0], 0.0f,
        s[1], u[1], -f[1], 0.0f,
        s[2], u[2], -f[2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    mat4_t translate_mat = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        -eye[0], -eye[1], -eye[2], 1.0f
    };

    mat4_t result;
    _tradmat4_mul(result, lookat_mat, translate_mat);

    for (int i = 0; i < 16; i++) {
        r[i] = result[i];
    }
}

// Function to set perspective projection matrix
void _tradmat4_perspective(mat4_t r, float fovy, float aspect, float znear, float zfar) {
    float tan_half_fovy = tanf(fovy / 2.0f);

    r[0] = 1.0f / (aspect * tan_half_fovy);
    r[1] = 0.0f;
    r[2] = 0.0f;
    r[3] = 0.0f;

    r[4] = 0.0f;
    r[5] = 1.0f / tan_half_fovy;
    r[6] = 0.0f;
    r[7] = 0.0f;

    r[8] = 0.0f;
    r[9] = 0.0f;
    r[10] = -(zfar + znear) / (zfar - znear);
    r[11] = -1.0f;

    r[12] = 0.0f;
    r[13] = 0.0f;
    r[14] = -(2.0f * zfar * znear) / (zfar - znear);
    r[15] = 0.0f;
}

// Function to set orthographic projection matrix
void _tradmat4_ortho(mat4_t r, float left, float right, float bottom, float top, float znear, float zfar) {
    r[0] = 2.0f / (right - left);
    r[1] = 0.0f;
    r[2] = 0.0f;
    r[3] = 0.0f;

    r[4] = 0.0f;
    r[5] = 2.0f / (top - bottom);
    r[6] = 0.0f;
    r[7] = 0.0f;

    r[8] = 0.0f;
    r[9] = 0.0f;
    r[10] = -2.0f / (zfar - znear);
    r[11] = 0.0f;

    r[12] = -(right + left) / (right - left);
    r[13] = -(top + bottom) / (top - bottom);
    r[14] = -(zfar + znear) / (zfar - znear);
    r[15] = 1.0f;
}

// Function to translate a 4x4 matrix
void _tradmat4_translate(mat4_t r, const vec3_t t) {
    mat4_t translate_mat = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        t[0], t[1], t[2], 1.0f
    };

    mat4_t result;
    _tradmat4_mul(result, r, translate_mat);

    for (int i = 0; i < 16; i++) {
        r[i] = result[i];
    }
}

// Function to rotate a 4x4 matrix
void _tradmat4_rotate(mat4_t r, float angle, const vec3_t axis) {
    float c = cosf(angle);
    float s = sinf(angle);
    float one_minus_c = 1.0f - c;

    vec3_t normalized_axis;
    _tradvec3_normalize(normalized_axis, axis);

    float x = normalized_axis[0];
    float y = normalized_axis[1];
    float z = normalized_axis[2];

    mat4_t rotate_mat = {
        x * x * one_minus_c + c,     x * y * one_minus_c - z * s, x * z * one_minus_c + y * s, 0.0f,
        y * x * one_minus_c + z * s, y * y * one_minus_c + c,     y * z * one_minus_c - x * s, 0.0f,
        z * x * one_minus_c - y * s, z * y * one_minus_c + x * s, z * z * one_minus_c + c,     0.0f,
        0.0f,                        0.0f,                        0.0f,                        1.0f
    };

    mat4_t result;
    _tradmat4_mul(result, r, rotate_mat);

    for (int i = 0; i < 16; i++) {
        r[i] = result[i];
    }
}

// Function to transpose a 4x4 matrix
void _tradmat4_transpose(mat4_t r, const mat4_t m) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            r[i * 4 + j] = m[j * 4 + i];
        }
    }
}

// Function to invert a 4x4 matrix
bool _tradmat4_inverse(mat4_t r, const mat4_t m) {
    float inv[16], det;
    int i;

    inv[0] = m[5] * m[10] * m[15] -
        m[5] * m[11] * m[14] -
        m[9] * m[6] * m[15] +
        m[9] * m[7] * m[14] +
        m[13] * m[6] * m[11] -
        m[13] * m[7] * m[10];

    inv[4] = -m[4] * m[10] * m[15] +
        m[4] * m[11] * m[14] +
        m[8] * m[6] * m[15] -
        m[8] * m[7] * m[14] -
        m[12] * m[6] * m[11] +
        m[12] * m[7] * m[10];

    inv[8] = m[4] * m[9] * m[15] -
        m[4] * m[11] * m[13] -
        m[8] * m[5] * m[15] +
        m[8] * m[7] * m[13] +
        m[12] * m[5] * m[11] -
        m[12] * m[7] * m[9];

    inv[12] = -m[4] * m[9] * m[14] +
        m[4] * m[10] * m[13] +
        m[8] * m[5] * m[14] -
        m[8] * m[6] * m[13] -
        m[12] * m[5] * m[10] +
        m[12] * m[6] * m[9];

    inv[1] = -m[1] * m[10] * m[15] +
        m[1] * m[11] * m[14] +
        m[9] * m[2] * m[15] -
        m[9] * m[3] * m[14] -
        m[13] * m[2] * m[11] +
        m[13] * m[3] * m[10];

    inv[5] = m[0] * m[10] * m[15] -
        m[0] * m[11] * m[14] -
        m[8] * m[2] * m[15] +
        m[8] * m[3] * m[14] +
        m[12] * m[2] * m[11] -
        m[12] * m[3] * m[10];

    inv[9] = -m[0] * m[9] * m[15] +
        m[0] * m[11] * m[13] +
        m[8] * m[1] * m[15] -
        m[8] * m[3] * m[13] -
        m[12] * m[1] * m[11] +
        m[12] * m[3] * m[9];

    inv[13] = m[0] * m[9] * m[14] -
        m[0] * m[10] * m[13] -
        m[8] * m[1] * m[14] +
        m[8] * m[2] * m[13] +
        m[12] * m[1] * m[10] -
        m[12] * m[2] * m[9];

    inv[2] = m[1] * m[6] * m[15] -
        m[1] * m[7] * m[14] -
        m[5] * m[2] * m[15] +
        m[5] * m[3] * m[14] +
        m[13] * m[2] * m[7] -
        m[13] * m[3] * m[6];

    inv[6] = -m[0] * m[6] * m[15] +
        m[0] * m[7] * m[14] +
        m[4] * m[2] * m[15] -
        m[4] * m[3] * m[14] -
        m[12] * m[2] * m[7] +
        m[12] * m[3] * m[6];

    inv[10] = m[0] * m[5] * m[15] -
        m[0] * m[7] * m[13] -
        m[4] * m[1] * m[15] +
        m[4] * m[3] * m[13] +
        m[12] * m[1] * m[7] -
        m[12] * m[3] * m[5];

    inv[14] = -m[0] * m[5] * m[14] +
        m[0] * m[6] * m[13] +
        m[4] * m[1] * m[14] -
        m[4] * m[2] * m[13] -
        m[12] * m[1] * m[6] +
        m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
        m[1] * m[7] * m[10] +
        m[5] * m[2] * m[11] -
        m[5] * m[3] * m[10] -
        m[9] * m[2] * m[7] +
        m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
        m[0] * m[7] * m[10] -
        m[4] * m[2] * m[11] +
        m[4] * m[3] * m[10] +
        m[8] * m[2] * m[7] -
        m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
        m[0] * m[7] * m[9] +
        m[4] * m[1] * m[11] -
        m[4] * m[3] * m[9] -
        m[8] * m[1] * m[7] +
        m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
        m[0] * m[6] * m[9] -
        m[4] * m[1] * m[10] +
        m[4] * m[2] * m[9] +
        m[8] * m[1] * m[6] -
        m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0f / det;

    for (i = 0; i < 16; i++)
        r[i] = inv[i] * det;

    return true;
}

void _tradmat4_from_quat(mat4_t r, const quat_t q) {
    float x = q[0], y = q[1], z = q[2], w = q[3];
    float x2 = x + x, y2 = y + y, z2 = z + z;
    float xx = x * x2, xy = x * y2, xz = x * z2;
    float yy = y * y2, yz = y * z2, zz = z * z2;
    float wx = w * x2, wy = w * y2, wz = w * z2;

    r[0] = 1.0f - (yy + zz);
    r[1] = xy + wz;
    r[2] = xz - wy;
    r[3] = 0.0f;

    r[4] = xy - wz;
    r[5] = 1.0f - (xx + zz);
    r[6] = yz + wx;
    r[7] = 0.0f;

    r[8] = xz + wy;
    r[9] = yz - wx;
    r[10] = 1.0f - (xx + yy);
    r[11] = 0.0f;

    r[12] = 0.0f;
    r[13] = 0.0f;
    r[14] = 0.0f;
    r[15] = 1.0f;
}

void _tradmat4_to_quat(quat_t q, const mat4_t m) {
    float trace = m[0] + m[5] + m[10];
    if (trace > 0.0f) {
        float s = 0.5f / sqrtf(trace + 1.0f);
        q[3] = 0.25f / s;
        q[0] = (m[9] - m[6]) * s;
        q[1] = (m[2] - m[8]) * s;
        q[2] = (m[4] - m[1]) * s;
    }
    else {
        if (m[0] > m[5] && m[0] > m[10]) {
            float s = 2.0f * sqrtf(1.0f + m[0] - m[5] - m[10]);
            q[3] = (m[9] - m[6]) / s;
            q[0] = 0.25f * s;
            q[1] = (m[1] + m[4]) / s;
            q[2] = (m[2] + m[8]) / s;
        }
        else if (m[5] > m[10]) {
            float s = 2.0f * sqrtf(1.0f + m[5] - m[0] - m[10]);
            q[3] = (m[2] - m[8]) / s;
            q[0] = (m[1] + m[4]) / s;
            q[1] = 0.25f * s;
            q[2] = (m[6] + m[9]) / s;
        }
        else {
            float s = 2.0f * sqrtf(1.0f + m[10] - m[0] - m[5]);
            q[3] = (m[4] - m[1]) / s;
            q[0] = (m[2] + m[8]) / s;
            q[1] = (m[6] + m[9]) / s;
            q[2] = 0.25f * s;
        }
    }
}

float _tradmat4_determinant(const mat4_t m) {
    float det;

    det = m[0] * (m[5] * (m[10] * m[15] - m[14] * m[11]) -
        m[6] * (m[9] * m[15] - m[13] * m[11]) +
        m[7] * (m[9] * m[14] - m[13] * m[10]));

    det -= m[1] * (m[4] * (m[10] * m[15] - m[14] * m[11]) -
        m[6] * (m[8] * m[15] - m[12] * m[11]) +
        m[7] * (m[8] * m[14] - m[12] * m[10]));

    det += m[2] * (m[4] * (m[9] * m[15] - m[13] * m[11]) -
        m[5] * (m[8] * m[15] - m[12] * m[11]) +
        m[7] * (m[8] * m[13] - m[12] * m[9]));

    det -= m[3] * (m[4] * (m[9] * m[14] - m[13] * m[10]) -
        m[5] * (m[8] * m[14] - m[12] * m[10]) +
        m[6] * (m[8] * m[13] - m[12] * m[9]));
    return det;
}

bool _tradmat4_inverse_affine(mat4_t r, const mat4_t m) {
    // Calculate the inverse of the rotation part (upper 3x3 matrix)
    float det = m[0] * (m[5] * m[10] - m[9] * m[6]) -
        m[1] * (m[4] * m[10] - m[8] * m[6]) +
        m[2] * (m[4] * m[9] - m[8] * m[5]);

    if (fabs(det) < 1e-6) {
        // Matrix is singular and cannot be inverted
        return false;
    }

    float inv_det = 1.0f / det;

    r[0] = (m[5] * m[10] - m[9] * m[6]) * inv_det;
    r[1] = (m[2] * m[9] - m[1] * m[10]) * inv_det;
    r[2] = (m[1] * m[6] - m[2] * m[5]) * inv_det;
    r[3] = 0.0f;

    r[4] = (m[8] * m[6] - m[4] * m[10]) * inv_det;
    r[5] = (m[0] * m[10] - m[2] * m[8]) * inv_det;
    r[6] = (m[2] * m[4] - m[0] * m[6]) * inv_det;
    r[7] = 0.0f;

    r[8] = (m[4] * m[9] - m[8] * m[5]) * inv_det;
    r[9] = (m[1] * m[8] - m[0] * m[9]) * inv_det;
    r[10] = (m[0] * m[5] - m[1] * m[4]) * inv_det;
    r[11] = 0.0f;

    // Inverse translation
    r[12] = -(r[0] * m[12] + r[4] * m[13] + r[8] * m[14]);
    r[13] = -(r[1] * m[12] + r[5] * m[13] + r[9] * m[14]);
    r[14] = -(r[2] * m[12] + r[6] * m[13] + r[10] * m[14]);
    r[15] = 1.0f;

    return true;
}

void _tradmat4_forward(vec3_t forward, const mat4_t m) {
    forward[0] = m[8];
    forward[1] = m[9];
    forward[2] = m[10];
}

// Function to get the up vector from a 4x4 matrix
void _tradmat4_up(vec3_t up, const mat4_t m) {
    up[0] = m[4];
    up[1] = m[5];
    up[2] = m[6];
}

// Function to get the right vector from a 4x4 matrix
void _tradmat4_right(vec3_t right, const mat4_t m) {
    right[0] = m[0];
    right[1] = m[1];
    right[2] = m[2];
}

// Function to get the position from a 4x4 matrix
void _tradmat4_position(vec3_t position, const mat4_t m) {
    position[0] = m[12];
    position[1] = m[13];
    position[2] = m[14];
}

void _tradmat4_test() {
    mat4_t model = mat4_identity;
    mat4_t view = mat4_identity;
    mat4_t projection = mat4_identity;
    vec3_t scale = { 2.0f, 2.0f, 2.0f };
    vec3_t position = { 1.0f, 2.0f, 3.0f };
    vec3_t eye = { 0.0f, 0.0f, 5.0f };
    vec3_t center = { 0.0f, 0.0f, 0.0f };
    vec3_t up = { 0.0f, 1.0f, 0.0f };
    float fovy = (float)M_PI / 4.0f; // 45 degrees
    float aspect = 16.0f / 9.0f;
    float znear = 0.1f;
    float zfar = 100.0f;
    vec3_t axis = { 0.0f, 1.0f, 0.0f };
    float angle = (float)M_PI / 4.0f; // 45 degrees

    // Set the scale
    _tradmat4_setscale(model, scale);
    print_mat4("Model matrix after scaling", model);

    // Set the position
    _tradmat4_setpos(model, position);
    print_mat4("Model matrix after translation", model);

    // Rotate the model matrix
    _tradmat4_rotate(model, angle, axis);
    print_mat4("Model matrix after rotation", model);

    // Translate the model matrix
    vec3_t translation = { 1.0f, 1.0f, 1.0f };
    _tradmat4_translate(model, translation);
    print_mat4("Model matrix after translation", model);

    // Create a lookat matrix
    _tradmat4_lookat(view, eye, center, up);
    print_mat4("View matrix", view);

    // Create a perspective projection matrix
    _tradmat4_perspective(projection, fovy, aspect, znear, zfar);
    print_mat4("Projection matrix", projection);

    // Create an orthographic projection matrix
    _tradmat4_ortho(projection, -1.0f, 1.0f, -1.0f, 1.0f, znear, zfar);
    print_mat4("Orthographic projection matrix", projection);

    // Transpose the model matrix
    mat4_t transpose;
    _tradmat4_transpose(transpose, model);
    print_mat4("Transpose of model matrix", transpose);

    // Invert the model matrix
    mat4_t inverse;
    if (_tradmat4_inverse(inverse, model)) {
        print_mat4("Inverse of model matrix", inverse);
    }
    else {
        printf("Model matrix is not invertible\n");
    }

    // Determine the determinant of the model matrix
    float det = _tradmat4_determinant(model);
    printf("Determinant of model matrix: %f\n", det);

    // Invert the model matrix (affine)
    if (_tradmat4_inverse_affine(inverse, model)) {
        print_mat4("Affine inverse of model matrix", inverse);
    }
    else {
        printf("Affine model matrix is not invertible\n");
    }
}
