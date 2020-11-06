#version 450

layout(location=0) out vec4 color;

// for an obscure reason, the gl_Position
// from previous stage goes here when compiling
// as HLSL
layout(location=0) in vec4 fragPos;

layout(location=1) in VertOut {
    vec3 normal;
    vec4 frag_pos;
    vec2 uv;
    vec3 camera_position;
    vec3 light_position;
} vertOut;

layout(binding = 0) uniform sampler2D diffuseTexture;
layout(binding = 1) uniform sampler2D normalTexture;

void main() {
    vec3 position = vertOut.frag_pos.xyz;
    vec3 normal = vertOut.normal;
    vec3 abeldo = texture(diffuseTexture, vertOut.uv).rgb;

    vec3 lighting = abeldo * 0.3;
    vec3 view_dir = normalize(vertOut.camera_position - position);

    vec3 light_dir = normalize(vertOut.light_position - position);
    vec3 diffuse = max(dot(normal, light_dir), 0.0) * abeldo * vec3(1.0, 1.0, 1.0);
    lighting += diffuse;

    color = vec4(lighting, 1.0);
}
