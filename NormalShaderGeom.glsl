#version 460
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;
in VS_OUT {
	vec3 normal;
	vec3 fragPos;
} gs_in[];
out VS_OUT {
	vec3 normal;
	vec3 fragPos;
} gs_out;

void main()
{
	for (int i = 0; i < 3; i++)
	{
		gs_out.normal = gs_in[i].normal;
		gs_out.fragPos = gs_in[i].fragPos;

		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
		gl_Position = gl_in[i].gl_Position + vec4(0.2 * gs_in[i].normal, 0);
		EmitVertex();
		EndPrimitive();
	}
}