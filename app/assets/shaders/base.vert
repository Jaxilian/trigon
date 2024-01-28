#version 450

layout(location = 0) in vec3 inPos;      // Position of the vertex
layout(location = 1) in vec3 inColor;    // Color of the vertex

layout(set = 0, binding = 0) uniform global_buffer {
    vec2 window_extent; 
    mat4 camera_transform;
    mat4 camera_projection;
} global;

layout(set = 1, binding = 0) uniform matrix_buffer {
    mat4 matrices[64];
} transform;

layout(location = 0) out vec3 fragColor; // Pass color to fragment shader

void main() {
    // Apply transformation matrices to the vertex position
    gl_Position = global.camera_projection * global.camera_transform * vec4(inPos, 1.0);

    // Pass the color to the fragment shader
    fragColor = inColor;
}