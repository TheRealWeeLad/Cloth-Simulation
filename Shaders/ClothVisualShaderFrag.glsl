#version 460 core
in VS_OUT {
	vec3 movement;
	vec4 movedPos;
} fs_in;
out vec4 FragColor;

void main()
{
	// Exponential to scale between white and red
	FragColor = vec4(1) - vec4(0, 1, 2, 0) * exp(-1 / (500 * length(fs_in.movement)));
}