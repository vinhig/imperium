#version 450

layout(location = 0) out vec4 outFragColor;

layout (location = 0) in vec3 outColor;

void main() { outFragColor = vec4(outColor, 1.0f); }