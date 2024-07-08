#version 450

#ifdef VERTEX_SHADER
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec3 frag_color;

layout(set = 0, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    frag_color = in_color;
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_position, 1.0);
}
#endif

#ifdef FRAGMENT_SHADER
layout(location = 0) in vec3 frag_color;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(frag_color, 1.0);
}
#endif