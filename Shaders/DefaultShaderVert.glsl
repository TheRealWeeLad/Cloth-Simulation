#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
out VS_OUT {
	vec3 normal;
	vec3 fragPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
	gl_Position = project * view * model * vec4(pos, 1.0);
	// Send world space coords to frag shader
	vs_out.fragPos = vec3(model * vec4(pos, 1.0));
	vs_out.normal = mat3(transpose(inverse(model))) * normal;
}