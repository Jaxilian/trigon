#version 450

layout(location = 0) in vec2 inPosition; // Vertex position
layout(location = 1) in vec2 inTexCoord; // Texture coordinate

layout(location = 0) out vec2 fragTexCoord;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 projection;
} ubo;

void main() {
    gl_Position = ubo.projection * vec4(inPosition, 0.0, 1.0);
    fragTexCoord = inTexCoord;
}
