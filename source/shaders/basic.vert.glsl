#version 450

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;

layout(location=0) out vec4 o_color;
// layout(location=1) out float o_ambient;
// layout(location=2) out float o_specular;
//
// layout(location=3) out vec3 o_camera_position;
// layout(location=4) out vec3 o_light_position[3];

// layout(location=5) out vec3 o_normal;

layout(std140, binding=0) uniform Material {
    vec4 diffuse_color;
    // float ambient;
    // float specular;
    mat4 mvp;
};

layout(std140, binding=1) uniform Lights {
    vec3 camera_position;
    vec3 light_position[3];
};

void main() {
    o_color = vec4(normal, 1.0);
    // o_ambient = ambient;
    // o_specular = specular;

    // o_camera_position = camera_position;
    // o_light_position = light_position;

    gl_Position = mvp * vec4(position, 1.0);
    // o_normal = normal;
}
