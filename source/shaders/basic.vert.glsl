#version 450

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;
layout(location=3) in float material;

layout(location=1) out VertOut {
    vec2 uv;
    vec3 color;
    float ambient;
    float specular;
    vec3 camera_position;
    vec3 light_position;
    vec3 normal;
    vec4 frag_pos;
} vertOut;

layout(location=9) flat out int texture_id;

layout(std140, binding=0) uniform Object {
    mat4 mvp;
    mat4 model;
} object;

layout(std140, binding=1) uniform Lights {
    vec3 camera_position;
    vec3 light_position;
};

struct FuckGlsl {
    float ambient;
    float specular;
    float texture;
    float dumb; // TODO: opengl glsl compiler seems to convert this to vec3,
                // therefore we're in a stride of 4, so if we do not have this
                // dumb variable, nothing work
};

layout(std140, binding=2) uniform Material {
    FuckGlsl m[3];
} materials;

void main() {
    vertOut.uv = uv;
    vertOut.color = vec3(materials.m[int(material)].texture, materials.m[int(material)].texture, materials.m[int(material)].texture);
    // vertOut.color = vec3(1.0, 1.0, 1.0);
    vertOut.normal = mat3(transpose(inverse(object.model))) * normal;;

    vertOut.camera_position = camera_position;
    vertOut.light_position = light_position;
    vertOut.frag_pos = object.mvp * vec4(position, 1.0);

    vertOut.ambient = materials.m[int(material)].ambient;
    vertOut.specular = materials.m[int(material)].specular;
    texture_id = int(materials.m[int(material)].texture);

    /*matrices.mvp = mvp;
    matrices.model = model;*/

    gl_Position = object.mvp * vec4(position, 1.0);
}
