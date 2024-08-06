#ifndef TR_DEFAULT_EXAMPLE_H
#define TR_DEFAULT_EXAMPLE_H

char vertex_shader_code[] = R"(
#version 450

#ifdef VERTEX_SHADER
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec2 frag_uv;

layout(set = 0, binding = 0) uniform MVP {
    mat4 model;
    mat4 view;
    mat4 proj;
} mvp;

void main() {
    frag_uv = in_uv;
    gl_Position = mvp.proj * mvp.view * mvp.model * vec4(in_position, 1.0);
}
#endif

#ifdef FRAGMENT_SHADER
layout(location = 0) in vec2 frag_uv;
layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 1) uniform sampler2D texture_sampler;

void main() {
    out_color = texture(texture_sampler, frag_uv);
}
#endif
)";

#endif // !TR_DEFAULT_EXAMPLE_H

