#version 450

layout(location=0) in vec3 position;

layout (std140, binding=1) uniform Light {
    mat4 viewProj;
    vec4 position;
} light;

layout(std140, binding=2) uniform Object {
    mat4 model;
} object;

void main() {
    gl_Position = light.viewProj * object.model * vec4(position, 1.0);
}
