#version 450

layout(location = 0) in vec2 fragUV;  // UV from vertex shader
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec4 outColor; // Output color

// If using texture, uncomment the following lines
// uniform sampler2D texSampler;
// void main() {
//     outColor = texture(texSampler, fragUV);
// }


void main() {
    float transparency = 1.0;
    vec3 pixel_color = in_color;

    outColor = vec4(in_color, transparency); 
}