#version 460 core

struct Material
{
	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	bool two_sided;
};

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};