#pragma once
#include "GameObject.h"
#include "Physics.h"
#include "ClothCollider.h"
#include <vector>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 lastPosition;
	glm::vec3 velocity;
	glm::vec3 springForce;

	Vertex(glm::vec3 position)
	{
		this->position = position;
		this->lastPosition = position;
		this->velocity = {};
		this->springForce = {};
	}
};

enum SimulationMethod {
	SpringMass, PBD
};

class Cloth
{
private:
	static const unsigned long DEFAULT_RESOLUTION{ 10 };
	unsigned int resolution; // Number of vertices per side (assuming square)
	unsigned int numParticles;
	const float springConstant{ 500 };
	const float bendingSpringConstant{ springConstant * 0.2F };
	const float springDamping{ 5 };
	const float airResistivity{ 1 };
	const float clothThickness{ 0.1F };
	const float restLen;
	const float shearRestLen;
	// For DEBUG
	const unsigned long anchors[4];

	bool adjacent(int idx1, int idx2) const;

	// Simulation
	void SpringMassSimulationStep(int idx);
	void UpdateNormals(int idx);
	void HandleCollisions(int idx);
public:
	std::vector<Vertex> vertices; // 2D array of vertices (flattened)
	Mesh mesh;
	MeshRenderer* meshRenderer;
	ClothCollider collider;
	glm::vec3 position;

	Cloth();
	Cloth(unsigned int resolution, SimulationMethod method = SpringMass);
	Cloth(glm::vec3 position, SimulationMethod method = SpringMass);
	Cloth(glm::vec3 position, float scale, unsigned int resolution = DEFAULT_RESOLUTION, SimulationMethod = SpringMass);
	
	void setRenderer(MeshRenderer* renderer);
	void meshUpdate();
	void fixedUpdate();
};