#version 450 core

layout(location = 0) in vec3 position;

layout(std140, binding = 0) uniform Material {
    vec4 color;
};

layout(location = 0) out vec4 o_color;

void main() {
    o_color = color;
    gl_Position = vec4(position, 1.0);
}
