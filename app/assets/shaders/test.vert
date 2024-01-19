#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inUV;

layout(set = 0, binding = 0) uniform global_buffer {
    vec2 window_extent; 
} global;

layout(set = 1, binding = 0) uniform matrix_buffer {
    mat4 matrices[1000];
} transform;

layout(location = 0) out vec2 fragUV;

void main() {
    mat4 transMatrix = transform.matrices[gl_InstanceIndex];


    float aspectRatio = global.window_extent.x / global.window_extent.y;

    vec2 scale = aspectRatio > 1.0 ?
                 vec2(1.0 / aspectRatio, 1.0) : 
                 vec2(1.0, aspectRatio); 


    vec4 pos4 = vec4(inPos * scale, 0.0, 1.0);
    vec4 transformedPos = transMatrix * pos4;

    gl_Position = transformedPos;
    fragUV = inUV;
}