#version 450

layout(location=0) out vec4 color;

layout(location=0) in vec2 o_uv;

layout(binding = 0) uniform sampler2D position;
layout(binding = 1) uniform sampler2D normal;
layout(binding = 2) uniform sampler2D abeldo;
layout(binding = 3) uniform sampler2D specular;

void main() {
    color = texture(position, o_uv)
        * texture(normal, o_uv)
        // * texture(abeldo, o_uv)
        * texture(specular, o_uv);
}
