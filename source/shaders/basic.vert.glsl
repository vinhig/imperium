#version 450

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;

layout(location=1) out VertOut {
    vec3 normal;
    vec4 frag_pos;
    vec2 uv;
    vec3 camera_position;
    vec3 light_position;
    vec3 shadow_coords;
} vertOut;

layout(std140, binding=0) uniform Camera {
    mat4 viewProj;
    vec4 position;
} camera;

layout (std140, binding=1) uniform Light {
    mat4 viewProj;
    vec4 position;
} light;

layout(std140, binding=2) uniform Object {
    mat4 model;
} object;

layout(std140, binding=3) uniform Material {
    float ambient;
    float specular;
    float color[4];
} material;

void main() {
    mat4 mvp = camera.viewProj * object.model;
    vertOut.normal = mat3(transpose(inverse(object.model))) * normal;
    vertOut.frag_pos = mvp * vec4(position, 1.0);
    vertOut.uv = uv;
    vertOut.camera_position = camera.position.xyz;
    vertOut.light_position = light.position.xyz;

    mat4 bias = mat4(
        vec4(0.5, 0.0, 0.0, 0.0),
        vec4(0.0, 0.5, 0.0, 0.0),
        vec4(0.0, 0.0, 0.5, 0.0),
        vec4(0.5, 0.5, 0.5, 1.0)
    );

    vertOut.shadow_coords = (bias * (light.viewProj * object.model * vec4(position, 1.0))).xyz;

    gl_Position = (mvp * vec4(position, 1.0));
}
