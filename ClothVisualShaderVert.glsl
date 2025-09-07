#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 3) in vec3 movement;
out VS_OUT {
	vec3 movement;
	vec4 movedPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
	vec3 extrudedPos = pos + vec3(0, 0, 0.001); // to avoid z-fighting
	gl_Position = project * view * model * vec4(extrudedPos, 1.0);
	// Send screen space coords to geometry shader
	vs_out.movement = movement;
	vs_out.movedPos = project * view * model * vec4(extrudedPos + normalize(movement) / 10, 1.0);
}