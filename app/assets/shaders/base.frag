#version 450

layout(location = 0) in vec3 fragColor; // Received color from vertex shader

layout(location = 0) out vec4 outColor; // Final color output

void main() {
    float transparency = 1.0; // Set transparency (alpha value)
    
    // Set the final output color
    outColor = vec4(fragColor, transparency); 
}