#version 450

layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;

layout(location=1) out VertOut {
    vec2 uv;
} vertOut;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    vertOut.uv = uv;
}