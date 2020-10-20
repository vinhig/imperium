#version 450

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;

layout(location=0) out FragIn {
    vec4 o_color;
    float o_ambient;
    float o_specular;
    vec3 o_camera_position;
    vec3 o_light_position[3];
    vec3 o_normal;
    vec3 o_frag_pos;
    vec2 o_uv;
} fragIn;

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
    fragIn.o_color = vec4(normal, 1.0);
    fragIn.o_ambient = ambient;
    fragIn.o_specular = specular;
    fragIn.o_camera_position = camera_position;
    fragIn.o_light_position[0] = light_position[0];
    fragIn.o_light_position[1] = light_position[1];
    fragIn.o_light_position[2] = light_position[2];
    fragIn.o_normal = mat3(transpose(inverse(model))) * normal;
    fragIn.o_frag_pos = vec3(model * vec4(position, 1.0));
    fragIn.o_uv = uv;
    gl_Position = mvp * vec4(position, 1.0);
}
