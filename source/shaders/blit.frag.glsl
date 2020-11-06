#version 450

layout(location=0) out vec4 color;

layout(binding = 0) uniform sampler2D t_position;
layout(binding = 1) uniform sampler2D t_normal;
layout(binding = 2) uniform sampler2D t_abeldo;
layout(binding = 3) uniform sampler2D t_specular;

layout(location=0) in VertOut {
    vec2 uv;
    vec3 light_position;
    vec3 camera_position;
} vertOut;

void main() {
    vec3 position = texture(t_position, vertOut.uv).rgb;
    vec3 normal = texture(t_normal, vertOut.uv).rgb;
    vec3 abeldo = texture(t_abeldo, vertOut.uv).rgb;

    vec3 lighting = abeldo * 0.1;
    vec3 view_dir = normalize(vertOut.camera_position - position);

    vec3 light_dir = normalize(vertOut.light_position - position);
    vec3 diffuse = max(dot(normal, light_dir), 0.0) * abeldo * vec3(1.0, 1.0, 1.0);
    lighting += diffuse;

    color = vec4(lighting, 1.0);
}
