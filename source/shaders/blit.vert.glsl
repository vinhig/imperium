#version 450

layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;

layout(location=0) out VertOut {
    vec2 uv;
    vec3 light_position;
    vec3 camera_position;
} vertOut;

layout (std140, binding=0) uniform Camera {
    mat4 viewProj;
    vec4 position;
} camera;

layout (std140, binding=1) uniform Light {
    mat4 viewProj;
    vec4 position;
} light;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    vertOut.uv = uv;
    vertOut.light_position = light.position.xyz;
    vertOut.camera_position = camera.position.xyz;
}
