#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../stb_image.h"
#include "vector"
#include "Shader.h"
#include "Camera.h"

struct Mesh
{
	const static Mesh CUBE;
	const static Mesh NULL_MESH;
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	std::vector<float> normals;
	std::vector<std::vector<float>> attributes;

	bool operator ==(Mesh rhs)
	{
		return &rhs == this;
	}
};

struct Texture
{
	unsigned int ID;
	const char* path;
	unsigned int horizontalWrap;
	unsigned int verticalWrap;
	glm::vec4 borderColor;
	unsigned int minFilter;
	unsigned int magFilter;
	unsigned int imgFormat; // JPG vs PNG -> RGB vs RGBA

	Texture(const char* path, unsigned int horizontalWrap, unsigned int verticalWrap, const glm::vec4& borderColor, unsigned int minFilter, unsigned int magFilter, unsigned int imgFormat)
		: path(path), horizontalWrap(horizontalWrap), verticalWrap(verticalWrap), borderColor(borderColor), minFilter(minFilter), magFilter(magFilter), imgFormat(imgFormat)
	{
		// Generate Texture ID
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizontalWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, verticalWrap);
		float borderColorArray[]{ borderColor.r, borderColor.g, borderColor.b, borderColor.a };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColorArray);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

		// Load texture image
		int width, height, nrChannels;
		unsigned char* data{ stbi_load(path, &width, &height, &nrChannels, 0) };

		if (data)
		{
			// Load image into texture
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, imgFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
		}

		// Free image memory
		stbi_image_free(data);
	}
};

class MeshRenderer
{
public:
	const static unsigned short ID{ 1 };
	Mesh mesh; // NEVER SET DIRECTLY
	glm::vec3 position;
	
	MeshRenderer();
	MeshRenderer(Mesh m);
	MeshRenderer(Mesh m, Shader s);
	MeshRenderer(Mesh m, Shader s, Material mat);
	MeshRenderer(Mesh m, Shader s, Material mat, std::vector<Texture> textures);

	void update();
	void render();
	void setMesh(Mesh m);
	void updateMesh(Mesh m);
	void setTextures(std::vector<Texture> textures);
	void addShader();
	void addShader(Shader s);
	void setMaterial(Material m);
	void setMatrices(glm::mat4 model, glm::mat4 view, glm::mat4 projection) const;
	unsigned int getVAO() const;
	unsigned int getVBO() const;
	unsigned int getEBO() const;

	static void UpdateAll();
private:
	static std::vector<MeshRenderer*> renderers;
	// For initialization purposes
	bool shaderTexturesAssigned{ false };

	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	std::vector<float> interleavedVertices;
	std::vector<Texture> textures;
	std::vector<Shader> shaders;

	void setupMesh();
	// Interleave vertex attributes to make vertex lookups more efficient
	void interleave();
	// Apply camera projection to shader
	void transformShader(int shaderIdx);
	// Update shader lighting
	void updateLighting(int shaderIdx);
};