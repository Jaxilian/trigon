#version 450

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D fontSampler;

layout(set = 0, binding = 2) uniform UniformBufferObject {
    vec4 textColor; // Text color
} ubo;

void main() {
    float alpha = texture(fontSampler, fragTexCoord).r;
    outColor = vec4(ubo.textColor.rgb, alpha * ubo.textColor.a);
}
