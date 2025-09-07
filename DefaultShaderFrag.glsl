#version 460 core
#include Materials.glsl

in VS_OUT {
	vec3 normal;
	vec3 fragPos;
} fs_in;
out vec4 FragColor;

uniform Material mat;
uniform Light light;

void main()
{
	// Ambient
	// http://devernay.free.fr/cours/opengl/materials.html
	float ambientIntensity = (0.212671 * mat.ambient.r + 0.715160 * mat.ambient.g + 
		0.072169 * mat.ambient.b) / (0.212671 * mat.diffuse.r +
		0.715160 * mat.diffuse.g + 0.072169 * mat.diffuse.b);
	vec3 ambient = ambientIntensity * light.ambient * mat.ambient;

	// Diffuse
	vec3 norm = normalize(fs_in.normal);
	if (mat.two_sided && !gl_FrontFacing) norm = -norm;
	vec3 lightDir = normalize(light.position - fs_in.fragPos);
	float diff = max(dot(norm, lightDir), 0);
	vec3 diffuse = diff * light.diffuse * mat.diffuse;

	vec3 result = (ambient + diffuse) * mat.color;
	FragColor = vec4(result, 1.0);
}