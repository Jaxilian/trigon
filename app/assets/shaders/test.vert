#version 450

layout(location = 0) in vec2 inPos;   
layout(location = 1) in vec2 inUV; 

layout(set = 0, binding = 0) uniform matrix_buffer {
    mat4 matrices[1000];
} transform;

layout(location = 0) out vec2 fragUV; 


void main() {
    mat4 transMatrix = transform.matrices[gl_InstanceIndex]; 

    vec3 pos3 = vec3(inPos, 1.0); 
    vec3 transformedPos = transMatrix * pos3; 

    gl_Position = vec4(transformedPos, 1.0); 
    fragUV = inUV;        
}