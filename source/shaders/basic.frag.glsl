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
    vec3 shadow_coords;
} vertOut;

// Textures from render graph always start at binding == 0
layout(binding = 0) uniform sampler2D shadowMap;

// Other textures comes after binding == 1
layout(binding = 1) uniform sampler2D diffuseTexture;
layout(binding = 2) uniform sampler2D normalTexture;

void main() {
    vec3 abeldo = texture(diffuseTexture, vertOut.uv).rgb;
    vec3 lighting = abeldo * 0.7;
    if (texture(shadowMap, vertOut.shadow_coords.xy).r < vertOut.shadow_coords.z - 0.001) {
        lighting *= 0.3;
//    } else {
//        vec3 position = vertOut.frag_pos.xyz;
//        vec3 normal = vertOut.normal;
//        vec3 view_dir = normalize(vertOut.camera_position - position);
//
//        vec3 light_dir = normalize(vertOut.light_position - position);
//        vec3 diffuse = max(dot(normal, light_dir), 0.0) * abeldo * vec3(1.0, 1.0, 1.0);
//        lighting += diffuse;
    }

    color = vec4(lighting, 1.0);
    // color = vec4(vertOut.shadow_coords.xy, 1.0, 1.0);
}
