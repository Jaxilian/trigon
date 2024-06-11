#pragma once
#include <xmmintrin.h> // SSE intrinsics
#include <pmmintrin.h> // SSE3 intrinsics
#include <cmath> // for std::sqrt, std::acos, std::atan2
#include <cstring> // for std::memcpy
#include "vector3.h"

static float IDENTITY[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

class matrix4_t {
public:
    matrix4_t() {
        identity();
    }

    matrix4_t(const matrix4_t& other) {
        std::memcpy(_data, other._data, sizeof(_data));
    }

    matrix4_t(const float* data) {
        std::memcpy(_data, data, sizeof(_data));
    }

    matrix4_t(float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33) {
        _data[0] = m00; _data[1] = m01; _data[2] = m02; _data[3] = m03;
        _data[4] = m10; _data[5] = m11; _data[6] = m12; _data[7] = m13;
        _data[8] = m20; _data[9] = m21; _data[10] = m22; _data[11] = m23;
        _data[12] = m30; _data[13] = m31; _data[14] = m32; _data[15] = m33;
    }

    inline void identity() {
        std::memcpy(_data, IDENTITY, sizeof(IDENTITY));
    }

    static inline matrix4_t mul(const matrix4_t& lhs, const matrix4_t& rhs) {
        alignas(16) float result[16];

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                __m128 sum = _mm_setzero_ps();
                for (int k = 0; k < 4; ++k) {
                    __m128 a = _mm_set1_ps(lhs._data[i + k * 4]);
                    __m128 b = _mm_load_ps1(&rhs._data[k + j * 4]);
                    sum = _mm_add_ps(sum, _mm_mul_ps(a, b));
                }
                _mm_store_ss(&result[i + j * 4], sum);
            }
        }

        return matrix4_t(result);
    }

    inline matrix4_t& operator*=(const matrix4_t& other) {
        *this = mul(*this, other);
        return *this;
    }

    inline matrix4_t operator*(const matrix4_t& other) const {
        return mul(*this, other);
    }

    void translate(const vector3_t& translation) {
        matrix4_t translation_matrix(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            translation.x, translation.y, translation.z, 1.0f
        );
        *this *= translation_matrix;
    }

    void scale(const vector3_t& scale) {
        matrix4_t scale_matrix(
            scale.x, 0.0f, 0.0f, 0.0f,
            0.0f, scale.y, 0.0f, 0.0f,
            0.0f, 0.0f, scale.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        *this *= scale_matrix;
    }

    void set_position(const vector3_t& position) {
        _data[12] = position.x;
        _data[13] = position.y;
        _data[14] = position.z;
    }

    void look_at(const vector3_t& look_target, const vector3_t& up = vector3_t(0.0f, 1.0f, 0.0f)) {
        vector3_t forward = (look_target - vector3_t(_data[12], _data[13], _data[14])).normalized();
        vector3_t right = vector3_t::cross(up, forward).normalized();
        vector3_t new_up = vector3_t::cross(forward, right);

        _data[0] = right.x;
        _data[1] = right.y;
        _data[2] = right.z;
        _data[4] = new_up.x;
        _data[5] = new_up.y;
        _data[6] = new_up.z;
        _data[8] = forward.x;
        _data[9] = forward.y;
        _data[10] = forward.z;
    }

    void rotate_euler(const vector3_t& degrees) {
        float rad_x = degrees.x * 0.0174532925f; // Convert to radians
        float rad_y = degrees.y * 0.0174532925f; // Convert to radians
        float rad_z = degrees.z * 0.0174532925f; // Convert to radians

        float cos_x = cosf(rad_x), sin_x = sinf(rad_x);
        float cos_y = cosf(rad_y), sin_y = sinf(rad_y);
        float cos_z = cosf(rad_z), sin_z = sinf(rad_z);

        matrix4_t rot_x(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, cos_x, -sin_x, 0.0f,
            0.0f, sin_x, cos_x, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        matrix4_t rot_y(
            cos_y, 0.0f, sin_y, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            -sin_y, 0.0f, cos_y, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        matrix4_t rot_z(
            cos_z, -sin_z, 0.0f, 0.0f,
            sin_z, cos_z, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        *this *= rot_z * rot_y * rot_x;
    }

    void print() const {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                std::cout << _data[i * 4 + j] << " ";
            }
            std::cout << std::endl;
        }
    }

    // Returns current forward vector
    vector3_t forward() const {
        return vector3_t(_data[8], _data[9], _data[10]).normalized();
    }

    // Returns current up vector
    vector3_t up() const {
        return vector3_t(_data[4], _data[5], _data[6]).normalized();
    }

    // Returns current right vector
    vector3_t right() const {
        return vector3_t(_data[0], _data[1], _data[2]).normalized();
    }

    // Returns current position
    vector3_t position() const {
        return vector3_t(_data[12], _data[13], _data[14]);
    }

    // Returns current scaling
    vector3_t scaling() const {
        return vector3_t(
            vector3_t(_data[0], _data[1], _data[2]).magnitude(),
            vector3_t(_data[4], _data[5], _data[6]).magnitude(),
            vector3_t(_data[8], _data[9], _data[10]).magnitude()
        );
    }

    // Returns current angles in Euler
    vector3_t euler_angles() const {
        float sy = sqrtf(_data[0] * _data[0] + _data[1] * _data[1]);

        bool singular = sy < 1e-6; // If

        float x, y, z;
        if (!singular) {
            x = atan2f(_data[6], _data[10]);
            y = atan2f(-_data[2], sy);
            z = atan2f(_data[1], _data[0]);
        }
        else {
            x = atan2f(-_data[9], _data[5]);
            y = atan2f(-_data[2], sy);
            z = 0;
        }
        return vector3_t(x * 57.2958f, y * 57.2958f, z * 57.2958f); // Convert to degrees
    }

private:
    alignas(16) float _data[16];
};
