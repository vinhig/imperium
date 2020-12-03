#version 450

layout(location=0) out vec4 color;

layout(location=0) in vec4 frag_pos;

layout(location=1) in VertOut {
    vec3 normal;
    vec4 frag_pos;
    vec2 uv;
    vec3 camera_position;
    vec3 light_position;
    vec3 shadow_coords;
} vertOut;

// Textures from render graph always start at binding == 0
layout(binding = 0) uniform sampler2D shadowMap;

// Other textures comes after binding == 1
layout(binding = 1) uniform sampler2D diffuseTexture;
layout(binding = 2) uniform sampler2D normalTexture;

void main() {
    vec3 abeldo = texture(diffuseTexture, vertOut.uv).rgb;
    vec3 ambient = abeldo * 0.7;

    if (texture(shadowMap, vertOut.shadow_coords.xy).r < vertOut.shadow_coords.z - 0.001) {
        ambient *= 0.3;
    } else {
        vec3 position = frag_pos.xyz;

        // Diffuse
        vec3 normal = normalize(vertOut.normal);
        vec3 light_dir =normalize(vertOut.light_position - position);
        float diff = max(dot(normal, light_dir), 0.0);
        vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

        // Specular
        float specularStrength = 0.5;
        vec3 view_dir = normalize(vertOut.camera_position - position);
        vec3 reflect_dir = reflect(-light_dir, normal);
        float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 16);
        vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);

        ambient = ambient + diffuse + specular;
    }

    color = vec4(ambient, 1.0);
    // color = vec4(vertOut.shadow_coords.xy, 1.0, 1.0);
}
