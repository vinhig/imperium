#version 450

layout(location=0) out vec4 color;

layout(location=0) in vec4 o_color;
// layout(location=1) in float o_ambient;
// layout(location=2) in float o_specular;
//
// layout(location=3) in vec3 o_camera_position;
// layout(location=4) in vec3 o_light_position[3];

// layout(location=5) in vec3 o_normal;

void main() {
    // Compute specular and light for each given
    // light position
    // for (int i = 0; i < 3; i++) {

    // }
    color = o_color;
}
