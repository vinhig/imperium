#version 450

layout(location=0) out vec4 color;

// for an obscure reason, the gl_Position
// from previous stage goes here when compiling
// as HLSL
// layout(location=0) in vec4 fragPos;

layout(location=1) in VertOut {
    vec3 color;
    float ambient;
    float specular;
    vec3 camera_position;
    vec3 light_position;
    vec2 uv;
    vec3 normal;
    vec3 fragPos;
} vertOut;

layout(binding = 0) uniform sampler2D diffuseTexture;
layout(binding = 1) uniform sampler2D normalTexture;

void main() {
    color = vec4(vertOut.uv, 1.0, 1.0);
    color = texture(diffuseTexture, vertOut.uv);
}
