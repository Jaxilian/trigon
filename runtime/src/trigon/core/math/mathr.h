#ifndef TG_MATHR_H
#define TG_MATHR_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // !M_PI


#define mat4_identity {1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f}

typedef float quat_t[4];
typedef float vec4_t[4];
typedef float vec3_t[3];
typedef float vec2_t[2];
typedef float mat4_t[16];

void mathr_unit_test();
void print_vec4(const char* label, const vec4_t v);
void print_vec3(const char* label, const vec3_t v);
void print_vec2(const char* label, const vec3_t v);
void print_mat4(const char* label, const mat4_t mat);


void _mathr_init();
#endif // !TG_MATHR_H
