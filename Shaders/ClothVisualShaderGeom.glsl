#version 460
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;
in VS_OUT {
	vec3 movement;
	vec4 movedPos;
} gs_in[];
out VS_OUT {
	vec3 movement;
	vec4 movedPos;
} gs_out;

void main()
{
	for (int i = 0; i < 3; i++)
	{
		gs_out.movement = gs_in[i].movement;
		gs_out.movedPos = gs_in[i].movedPos;

		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	
		gl_Position = gs_in[i].movedPos;
		EmitVertex();
		EndPrimitive();
	}
}