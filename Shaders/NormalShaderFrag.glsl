#version 460 core
#include Materials.glsl

in VS_OUT {
	vec3 normal;
	vec3 fragPos;
} fs_in;
out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0, 1.0, 0, 1.0);
}