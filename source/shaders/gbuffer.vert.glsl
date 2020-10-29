#version 450

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;

layout(location=1) out VertOut {
    vec3 normal;
    vec4 frag_pos;
    vec2 uv;
} vertOut;

layout(std140, binding=0) uniform Object {
    mat4 mvp;
    mat4 model;
} object;

void main() {
    vertOut.normal = mat3(transpose(inverse(object.model))) * normal;
    vertOut.frag_pos = object.mvp * vec4(position, 1.0);
    vertOut.uv = uv;

    gl_Position = object.mvp*vec4(position, 1.0);
}
