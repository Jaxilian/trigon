#version 450

layout(location = 0) in vec2 fragUV;  // UV from vertex shader

layout(location = 0) out vec4 outColor; // Output color

// If using texture, uncomment the following lines
// uniform sampler2D texSampler;
// void main() {
//     outColor = texture(texSampler, fragUV);
// }

// If not using texture, use a solid color
void main() {
    outColor = vec4(1.0, 1.0, 1.0, 1.0); // White color, replace with desired color
}