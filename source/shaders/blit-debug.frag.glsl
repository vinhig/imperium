#version 450

layout(location=0) out vec4 color;

layout(location=0) in VertOut {
    vec2 uv;
} vertOut;

layout(binding = 0) uniform sampler2D t_colorpass;
layout(binding = 1) uniform sampler2D t_debugpass;

void main() {
    vec4 debug = texture(t_debugpass, vertOut.uv);
    if (debug.x != 0.0 || debug.y != 0.0 || debug.z != 0.0) {
        color = debug;
    } else {
        color = texture(t_colorpass, vertOut.uv);
    }
}