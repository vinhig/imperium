#version 450

layout(location=0) out vec4 color;

layout(location=0) in vec4 o_color;

void main() {
    color = o_color;
}
