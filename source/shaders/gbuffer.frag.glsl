#version 450

layout(location=0) out vec4 position;
layout(location=1) out vec4 normal;
layout(location=2) out vec4 albedo;
layout(location=3) out vec4 specular;

layout(location=0) in vec4 frag_pos;

//layout(location=1) in VertOut {
//    vec3 normal;
//    vec2 uv;
//} vertOut;

layout(binding = 0) uniform sampler2D diffuseTexture;
//layout(binding = 1) uniform sampler2D normalTexture;

void main() {
    //    position = frag_pos;
    //    normal = vec4(vertOut.normal, 1.0);
    //    albedo = texture(diffuseTexture, vertOut.uv);
    //    specular = vec4(0.0, 0.0, 1.0, 1.0);
    position = frag_pos;
    normal = vec4(0.0, 1.0, 1.0, 1.0);
    albedo = vec4(0.0, 1.0, 0.0, 1.0);
    specular = vec4(1.0, 0.0, 1.0, 1.0);
}
