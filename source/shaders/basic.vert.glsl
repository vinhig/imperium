#version 450

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;

layout(location=1) out VertOut {
    vec2 uv;
    vec3 color;
    float ambient;
    float specular;
    vec4 camera_position;
    vec4 light_position;
    vec3 normal;
    vec4 frag_pos;
} vertOut;

layout(std140, binding=0) uniform Object {
    mat4 mvp;
    mat4 model;
} object;

layout(std140, binding=1) uniform Lights {
    vec4 camera_position;
    vec4 light_position;
};

layout(std140, binding=2) uniform Material {
    float ambient;
    float specular;
    float something;
} material;

void main() {
    vertOut.uv = uv;
    // vertOut.color = vec3(materials.m[int(material)].texture, materials.m[int(material)].texture, materials.m[int(material)].texture);
    vertOut.color = vec3(1.0, 1.0, 1.0);
    vertOut.normal = mat3(transpose(inverse(object.model))) * normal;;

    vertOut.camera_position = camera_position;
    vertOut.light_position = light_position;
    vertOut.frag_pos = object.mvp * vec4(position, 1.0);

    vertOut.ambient = material.ambient;
    vertOut.specular = material.specular;

    /*matrices.mvp = mvp;
    matrices.model = model;*/

    gl_Position = object.mvp * vec4(position, 1.0);
}
