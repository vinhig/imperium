#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vColor;

layout (location = 0) out vec3 outColor;

layout( push_constant ) uniform constants
{
	mat4 camera;
} Camera;

void main() 
{	
	gl_Position = Camera.camera * vec4(vPosition, 1.0f);
	outColor = vNormal;
}
