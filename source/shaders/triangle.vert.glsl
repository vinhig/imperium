#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vColor;

layout (location = 0) out vec3 outColor;

layout(set = 0, binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} cameraData;

layout(push_constant) uniform constants
{
    mat4 model;
} Model;

void main()
{
    gl_Position = cameraData.viewProj * Model.model * vec4(vPosition, 1.0f);
    outColor = vNormal;
}
