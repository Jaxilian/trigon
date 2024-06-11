#include <iostream>
#include <xmmintrin.h> 
#include <pmmintrin.h>
#include <cstring> 
#include <cmath>

class vector3_t {
public:
    vector3_t(float x_val, float y_val, float z_val)
        : x(_data[0]), y(_data[1]), z(_data[2]) {
        _data[0] = x_val;
        _data[1] = y_val;
        _data[2] = z_val;
        _data[3] = 0.0f; 
    }

    const float& x;
    const float& y;
    const float& z;

    inline vector3_t& operator+=(const vector3_t& other) {
        __m128 this_vec = _mm_load_ps(_data);
        __m128 other_vec = _mm_load_ps(other._data);
        __m128 result = _mm_add_ps(this_vec, other_vec);
        _mm_store_ps(_data, result);
        return *this;
    }

    inline vector3_t& operator-=(const vector3_t& other) {
        __m128 this_vec = _mm_load_ps(_data);
        __m128 other_vec = _mm_load_ps(other._data);
        __m128 result = _mm_sub_ps(this_vec, other_vec);
        _mm_store_ps(_data, result);
        return *this;
    }

    inline vector3_t& operator*=(const vector3_t& other) {
        __m128 this_vec = _mm_load_ps(_data);
        __m128 other_vec = _mm_load_ps(other._data);
        __m128 result = _mm_mul_ps(this_vec, other_vec);
        _mm_store_ps(_data, result);
        return *this;
    }

    inline vector3_t& operator/=(const vector3_t& other) {
        __m128 this_vec = _mm_load_ps(_data);
        __m128 other_vec = _mm_load_ps(other._data);
        __m128 result = _mm_div_ps(this_vec, other_vec);
        _mm_store_ps(_data, result);
        return *this;
    }

    inline vector3_t& operator=(const vector3_t& other) {
        if (this != &other) {
            std::memcpy(_data, other._data, sizeof(_data));
        }
        return *this;
    }

    inline vector3_t operator+(const vector3_t& other) const {
        vector3_t result(0.0f, 0.0f, 0.0f);
        __m128 this_vec = _mm_load_ps(_data);
        __m128 other_vec = _mm_load_ps(other._data);
        __m128 sum = _mm_add_ps(this_vec, other_vec);
        _mm_store_ps(result._data, sum);
        return result;
    }

    inline vector3_t operator-(const vector3_t& other) const {
        vector3_t result(0.0f, 0.0f, 0.0f);
        __m128 this_vec = _mm_load_ps(_data);
        __m128 other_vec = _mm_load_ps(other._data);
        __m128 diff = _mm_sub_ps(this_vec, other_vec);
        _mm_store_ps(result._data, diff);
        return result;
    }

    inline vector3_t operator*(const vector3_t& other) const {
        vector3_t result(0.0f, 0.0f, 0.0f);
        __m128 this_vec = _mm_load_ps(_data);
        __m128 other_vec = _mm_load_ps(other._data);
        __m128 prod = _mm_mul_ps(this_vec, other_vec);
        _mm_store_ps(result._data, prod);
        return result;
    }

    inline vector3_t operator/(const vector3_t& other) const {
        vector3_t result(0.0f, 0.0f, 0.0f);
        __m128 this_vec = _mm_load_ps(_data);
        __m128 other_vec = _mm_load_ps(other._data);
        __m128 quot = _mm_div_ps(this_vec, other_vec);
        _mm_store_ps(result._data, quot);
        return result;
    }

    inline vector3_t operator*(float scalar) const {
        vector3_t result(0.0f, 0.0f, 0.0f);
        __m128 this_vec = _mm_load_ps(_data);
        __m128 scalar_vec = _mm_set1_ps(scalar);
        __m128 prod = _mm_mul_ps(this_vec, scalar_vec);
        _mm_store_ps(result._data, prod);
        return result;
    }

    inline vector3_t& operator*=(float scalar) {
        __m128 this_vec = _mm_load_ps(_data);
        __m128 scalar_vec = _mm_set1_ps(scalar);
        __m128 result = _mm_mul_ps(this_vec, scalar_vec);
        _mm_store_ps(_data, result);
        return *this;
    }

    inline vector3_t& operator+=(float num) {
        __m128 this_vec = _mm_load_ps(_data);
        __m128 num_vec = _mm_set1_ps(num);
        __m128 result = _mm_add_ps(this_vec, num_vec);
        _mm_store_ps(_data, result);
        return *this;
    }

    inline vector3_t operator+(float num) const {
        vector3_t result(0.0f, 0.0f, 0.0f);
        __m128 this_vec = _mm_load_ps(_data);
        __m128 num_vec = _mm_set1_ps(num);
        __m128 sum = _mm_add_ps(this_vec, num_vec);
        _mm_store_ps(result._data, sum);
        return result;
    }

    inline vector3_t& operator-=(float num) {
        __m128 this_vec = _mm_load_ps(_data);
        __m128 num_vec = _mm_set1_ps(num);
        __m128 result = _mm_sub_ps(this_vec, num_vec);
        _mm_store_ps(_data, result);
        return *this;
    }

    inline vector3_t operator-(float num) const {
        vector3_t result(0.0f, 0.0f, 0.0f);
        __m128 this_vec = _mm_load_ps(_data);
        __m128 num_vec = _mm_set1_ps(num);
        __m128 sum = _mm_sub_ps(this_vec, num_vec);
        _mm_store_ps(result._data, sum);
        return result;
    }

    inline bool operator==(const vector3_t& other) const {
        return _data[0] == other._data[0] && _data[1] == other._data[1] && _data[2] == other._data[2];
    }

    inline bool operator!=(const vector3_t& other) const {
        return _data[0] != other._data[0] || _data[1] != other._data[1] || _data[2] != other._data[2];
    }

    inline vector3_t operator-() const {
        vector3_t result(0.0f, 0.0f, 0.0f);
        __m128 this_vec = _mm_load_ps(_data);
        __m128 neg_vec = _mm_sub_ps(_mm_setzero_ps(), this_vec);
        _mm_store_ps(result._data, neg_vec);
        return result;
    }

    inline float magnitude() const {
        __m128 this_vec = _mm_load_ps(_data);
        __m128 squared = _mm_mul_ps(this_vec, this_vec);
        __m128 sum = _mm_hadd_ps(squared, squared);
        sum = _mm_hadd_ps(sum, sum);
        return _mm_cvtss_f32(_mm_sqrt_ss(sum));
    }

    inline vector3_t normalized() const {
        float mag = magnitude();
        __m128 this_vec = _mm_load_ps(_data);
        __m128 mag_vec = _mm_set1_ps(mag);
        __m128 norm_vec = _mm_div_ps(this_vec, mag_vec);
        vector3_t result(0.0f, 0.0f, 0.0f);
        _mm_store_ps(result._data, norm_vec);
        return result;
    }

    inline float dot(const vector3_t& other) const {
        __m128 this_vec = _mm_load_ps(_data);
        __m128 other_vec = _mm_load_ps(other._data);
        __m128 prod = _mm_mul_ps(this_vec, other_vec);
        __m128 sum = _mm_hadd_ps(prod, prod);
        sum = _mm_hadd_ps(sum, sum);
        return _mm_cvtss_f32(sum);
    }

    inline vector3_t cross(const vector3_t& other) const {
        return vector3_t(
            _data[1] * other._data[2] - _data[2] * other._data[1],
            _data[2] * other._data[0] - _data[0] * other._data[2],
            _data[0] * other._data[1] - _data[1] * other._data[0]
        );
    }

    inline float angle_between(const vector3_t& other) const {
        float dot_prod = dot(other);
        float mags = magnitude() * other.magnitude();
        return std::acos(dot_prod / mags);
    }

    inline vector3_t projection_onto(const vector3_t& other) const {
        float dot_prod = dot(other);
        float other_mag_sq = other.magnitude() * other.magnitude();
        float scalar = dot_prod / other_mag_sq;
        return other * scalar;
    }

    inline float distance(const vector3_t& other) const {
        return (*this - other).magnitude();
    }

    // lerps from A to B where T is the normalized progression (0.0f-1.0f)
    static inline vector3_t lerp(const vector3_t& a, const vector3_t& b, float t) {
        __m128 t_vec = _mm_set1_ps(t);
        __m128 a_vec = _mm_load_ps(a._data);
        __m128 b_vec = _mm_load_ps(b._data);
        __m128 diff = _mm_sub_ps(b_vec, a_vec);
        __m128 t_diff = _mm_mul_ps(diff, t_vec);
        __m128 result = _mm_add_ps(a_vec, t_diff);

        vector3_t res(0.0f, 0.0f, 0.0f);
        _mm_store_ps(res._data, result);
        return res;
    }

private:
    alignas(16) float _data[4];
};

