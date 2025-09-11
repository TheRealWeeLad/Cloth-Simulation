#include "MeshRenderer.h"
#include "LightObject.h"

/* STATIC */
// Initialize Static Mesh Vertices
const Mesh Mesh::CUBE{
	// Vertices
	{-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f},
	{}, // Indices
	// Normals	
	{0, 0, -1,
	0, 0, -1,
	0, 0, -1,
	0, 0, -1,
	0, 0, -1,
	0, 0, -1,
	
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	
	-1, 0, 0,
	-1, 0, 0,
	-1, 0, 0,
	-1, 0, 0,
	-1, 0, 0,
	-1, 0, 0,
	
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	
	0, -1, 0,
	0, -1, 0,
	0, -1, 0,
	0, -1, 0,
	0, -1, 0,
	0, -1, 0,
	
	0, 1, 0,
	0, 1, 0,
	0, 1, 0,
	0, 1, 0,
	0, 1, 0,
	0, 1, 0}
};
const Mesh Mesh::NULL_MESH{};

std::vector<MeshRenderer*> MeshRenderer::renderers{};

/* PUBLIC */
MeshRenderer::MeshRenderer() : position{}
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	mesh = Mesh::NULL_MESH;
	addShader(); // use default shader
	renderers.push_back(this);
}
MeshRenderer::MeshRenderer(Mesh m) : MeshRenderer()
{
	setMesh(m);
}
MeshRenderer::MeshRenderer(Mesh m, Shader s) : MeshRenderer(m)
{
	shaders.clear();
	addShader(s);
}
MeshRenderer::MeshRenderer(Mesh m, Shader s, Material mat) : MeshRenderer(m, s)
{
	setMaterial(mat);
}
MeshRenderer::MeshRenderer(Mesh m, Shader s, Material mat, std::vector<Texture> textures) : MeshRenderer(m, s, mat)
{
	setTextures(textures);
}

void MeshRenderer::setMesh(Mesh m)
{
	mesh = m;
	setupMesh();
}
void MeshRenderer::updateMesh(Mesh m)
{
	mesh = m;
	glBindVertexArray(VAO);
	mesh.attributes.insert(mesh.attributes.begin(), mesh.normals);
	interleave();
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * interleavedVertices.size(), &interleavedVertices[0], GL_STATIC_DRAW);
}
void MeshRenderer::setTextures(std::vector<Texture> textures)
{
	this->textures = textures;
}
void MeshRenderer::addShader()
{
	shaders.push_back({ "DefaultShaderVert.glsl", "DefaultShaderFrag.glsl" });
}
void MeshRenderer::addShader(Shader s)
{
	shaders.push_back(s);
}
void MeshRenderer::setMaterial(Material m)
{
	shaders[0].setMaterial(m);
}

void MeshRenderer::setMatrices(glm::mat4 model, glm::mat4 view, glm::mat4 projection) const
{
	shaders[0].setMat4("model", model);
	shaders[0].setMat4("view", view);
	shaders[0].setMat4("projection", projection);
}

void MeshRenderer::update()
{
	for (int i = 0; i < shaders.size(); i++)
	{
		updateLighting(i);
		transformShader(i);
	}
	render();
}

void MeshRenderer::UpdateAll()
{
	for (int i = 0; i < renderers.size(); i++)
	{
		renderers[i]->update();
	}
}

void MeshRenderer::render()
{
	if (mesh == Mesh::NULL_MESH) return;

	// Initialize shader tex values on first pass through
	if (!shaderTexturesAssigned)
	{
		for (int i = 0; i < textures.size(); i++)
		{
			std::string texStr{ "Texture" };
			texStr.append(std::to_string(i + 1));
			shaders[0].setInt(texStr, i);
		}
		shaderTexturesAssigned = true;
	}

	// Render the mesh
	glBindVertexArray(VAO);
	// Activate textures
	if (!textures.empty())
	{
		for (int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i].ID);
		}
	}
	for (int i = 0; i < shaders.size(); i++)
	{
		// Activate shader
		shaders[i].use();

		// Draw shape
		if (mesh.indices.empty())
			glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size() / 3);
		else glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
	}
}

// Getters
unsigned int MeshRenderer::getVAO() const { return VAO; }
unsigned int MeshRenderer::getVBO() const { return VBO; }
unsigned int MeshRenderer::getEBO() const { return EBO; }

/* PRIVATE */
void MeshRenderer::setupMesh()
{
	glBindVertexArray(VAO);

	// Add normals to attributes
	mesh.attributes.insert(mesh.attributes.begin(), mesh.normals);

	// Interleave attribute data into vertex array
	interleave();

	// Setup VBO with vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * interleavedVertices.size(), &interleavedVertices[0], GL_STATIC_DRAW);
	// Setup EBO with indices
	if (!mesh.indices.empty())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.indices.size(), &mesh.indices[0], GL_STATIC_DRAW);
	}

	// Find stride of vertex attributes
	int stride{ sizeof(float) * 3 }; // Vertex size
	int numVertices = mesh.vertices.size() / 3;
	for (int i = 0; i < mesh.attributes.size(); i++)
		stride += sizeof(float) * mesh.attributes[i].size() / numVertices;
	// Store attribute data into VAO
	int offset{ 3 * sizeof(float) };
	// Vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	// Attributes
	for (int i = 0; i < mesh.attributes.size(); i++)
	{
		int size = mesh.attributes[i].size() / numVertices; // Use copy-initialization to avoid narrowing conversion error
		glVertexAttribPointer(i + 1, size, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		glEnableVertexAttribArray(i + 1);
		offset += sizeof(float) * size;
	}
}

/**
* @brief Interleaves all mesh attributes within its vertex array
* (Mesh attributes must be same length as vertex array)
* @returns new interleaved vertex array
*/
void MeshRenderer::interleave()
{
	interleavedVertices.clear();
	std::vector<int> attributeSizes{};
	int numVertices = mesh.vertices.size() / 3;
	for (int i = 0; i < numVertices; i++)
	{
		// Add vertex
		interleavedVertices.push_back(mesh.vertices[3 * i]);
		interleavedVertices.push_back(mesh.vertices[3 * i + 1]);
		interleavedVertices.push_back(mesh.vertices[3 * i + 2]);

		for (int j = 0; j < mesh.attributes.size(); j++)
		{
			std::vector<float> attrib{ mesh.attributes[j] };
			
			// Populate attrib sizes
			if (attributeSizes.size() < mesh.attributes.size())
				attributeSizes.push_back(attrib.size() / numVertices);

			// Add attribute data
			int attribSize{ attributeSizes[j] };
			for (int k = 0; k < attribSize; k++)
				interleavedVertices.push_back(attrib[attribSize * i + k]);
		}
	}
}

void MeshRenderer::transformShader(int shaderIdx)
{
	glm::mat4 model{ glm::mat4(1.0f) };
	model = glm::translate(model, position);
	//model = glm::rotate(model, time, glm::vec3(1.0f, 1.0f, 0));

	glm::mat4 projection{ glm::perspective(glm::radians(Camera::MainCamera->fov),
						800.0f / 600.0f, 0.1f, 100.0f) };

	shaders[shaderIdx].setMat4("model", model);
	shaders[shaderIdx].setMat4("view", Camera::MainCamera->view);
	shaders[shaderIdx].setMat4("project", projection);
}

void MeshRenderer::updateLighting(int shaderIdx)
{
	shaders[shaderIdx].setFloat3("light.position", LightObject::LightPos);
	shaders[shaderIdx].setFloat3("light.ambient", LightObject::LightMat.ambient);
	shaders[shaderIdx].setFloat3("light.diffuse", LightObject::LightMat.diffuse);
	shaders[shaderIdx].setFloat3("light.specular", LightObject::LightMat.specular);
}