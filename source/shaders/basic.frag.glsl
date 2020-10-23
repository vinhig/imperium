#version 450

layout(location=0) out vec4 color;

// for an obscure reason, the gl_Position
// from previous stage goes here when compiling
// as HLSL
layout(location=0) in vec4 fragPos;

layout(location=1) in VertOut {
    vec2 uv;
    vec3 color;
    float ambient;
    float specular;
    vec3 camera_position;
    vec3 light_position;
    vec3 normal;
    vec3 fragPos;
} vertOut;

layout(binding = 0) uniform sampler2D diffuseTexture;

void main() {
    color = texture(diffuseTexture, vertOut.uv);
}
