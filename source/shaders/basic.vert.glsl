#version 450

struct FragIn {
    vec4 o_color;
    float o_ambient;
    float o_specular;
    vec3 o_camera_position;
    vec3 o_light_position[3];
    vec3 o_normal;
    vec3 o_frag_pos;
};

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;

layout(location=0) out FragIn o_frag;

layout(std140, binding=0) uniform Material {
    vec4 diffuse_color;
    mat4 mvp;
    mat4 model;
    float ambient;
    float specular;
};

layout(std140, binding=1) uniform Lights {
    vec3 camera_position;
    vec3 light_position[3];
};

void main() {
    o_frag.o_color = vec4(normal, 1.0);
    o_frag.o_ambient = ambient;
    o_frag.o_specular = specular;
    o_frag.o_camera_position = camera_position;
    o_frag.o_light_position[0] = light_position[0];
    o_frag.o_light_position[1] = light_position[1];
    o_frag.o_light_position[2] = light_position[2];
    o_frag.o_normal = mat3(transpose(inverse(model))) * normal;
    o_frag.o_frag_pos = vec3(model * vec4(position, 1.0));
    gl_Position = mvp * vec4(position, 1.0);
}
