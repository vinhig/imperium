#version 450

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;
// layout(location=3) in int material;

layout(location=1) out VertOut {
    vec2 uv;
    vec3 color;
    float ambient;
    float specular;
    vec3 camera_position;
    vec3 light_position;
    vec3 normal;
    vec3 fragPos;
} vertOut;

layout(std140, binding=0) uniform Material {
    mat4 mvp;
    mat4 model;
    float ambient;
    float specular;
} material;

layout(std140, binding=1) uniform Lights {
    vec3 camera_position;
    vec3 light_position;
};

void main() {
    vertOut.color = normal;
    vertOut.ambient = material.ambient;
    vertOut.specular = material.specular;
    vertOut.camera_position = camera_position;
    vertOut.light_position = light_position;
    vertOut.uv = uv;
    vertOut.normal = normal;

    /*matrices.mvp = mvp;
    matrices.model = model;*/

    gl_Position = material.mvp * vec4(position, 1.0);;
}
