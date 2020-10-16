#version 450

layout(location=0) in vec3 position;
layout(location=1) in vec3 collor;

layout(location=0) out vec4 o_color;

layout(std140, binding = 0) uniform Material {
    vec4 color;
    mat4 mvp;
};

void main() {
    o_color = vec4(collor, 1.0);
    gl_Position = mvp * vec4(position, 1.0);
}
