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
    vec4 frag_pos;
} vertOut;

layout(binding = 0) uniform sampler2D diffuseTexture;

void main() {
    vec3 ambient = vec3(1.0, 1.0, 1.0) * vertOut.ambient;
    vec3 normal = normalize(vertOut.normal);
    vec3 textureColor = texture(diffuseTexture, vertOut.uv).xyz;

    vec3 light_dir = normalize(vertOut.light_position.xyz - vertOut.frag_pos.xyz);

    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

    vec3 view_dir = normalize(vertOut.camera_position.xyz - vertOut.frag_pos.xyz);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 64);
    vec3 specular = vertOut.specular * spec * vec3(1.0, 1.0, 1.0);

    color = vec4((ambient + diffuse + specular) * textureColor, 1.0);
    // color = vec4(vertOut.color, 1.0);
}
