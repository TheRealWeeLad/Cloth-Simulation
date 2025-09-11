#include "Cloth.h"

std::vector<Cloth*> Cloth::ClothObjects = {};

/* HELPER */
bool Cloth::adjacent(int idx1, int idx2) const
{
	int i = idx1 / resolution;
	int j = idx1 % resolution;
	int x = idx2 / resolution;
	int y = idx2 % resolution;

	if (abs(i - x) <= 1 && abs(j - y) <= 1) return true;
	return false;
}

/* THE REST*/
Cloth::Cloth() : Cloth(DEFAULT_RESOLUTION) {}
Cloth::Cloth(unsigned int resolution, SimulationMethod method) : Cloth({}, 1, resolution, method) {}
Cloth::Cloth(glm::vec3 position, SimulationMethod method) : Cloth(position, 1, DEFAULT_RESOLUTION, method) {}
Cloth::Cloth(glm::vec3 position, float scale, unsigned int resolution, SimulationMethod method) : position(position),
	resolution(resolution), numParticles(resolution * resolution), restLen(scale / (resolution - 1)), 
	shearRestLen(scale* sqrt(2) / (resolution - 1)), anchors{17, 19, 10, 12},
	collider{ (int)numParticles, clothThickness }, meshRenderer()
{
	ClothObjects.push_back(this);
	// Create a square mesh in the xz-plane
	std::vector<float> vertexMovement(numParticles * 3);
	std::vector<float> texCoords{};
	for (unsigned int i = 0; i < resolution; i++) {
		for (unsigned int j = 0; j < resolution; j++) {
			glm::vec3 pos{ position.x - scale * 0.5F + j * restLen, position.y, position.z - scale * 0.5 + i * restLen };
			vertices.push_back({ pos });
			mesh.vertices.push_back(pos.x);
			mesh.vertices.push_back(pos.y);
			mesh.vertices.push_back(pos.z);
			mesh.normals.push_back(0);
			mesh.normals.push_back(1);
			mesh.normals.push_back(0);
			texCoords.push_back((float)i / resolution);
			texCoords.push_back((float)j / resolution);
		}
	}
	mesh.attributes.push_back(texCoords);
	mesh.attributes.push_back(vertexMovement);
	// Set mesh indices
	for (int i = 0; i < resolution - 1; i++) {
		for (int j = 0; j < resolution - 1; j++) {
			int current_vertex = i * resolution + j;
			mesh.indices.push_back(current_vertex);
			mesh.indices.push_back(current_vertex + 1);
			mesh.indices.push_back(current_vertex + resolution);
			mesh.indices.push_back(current_vertex + 1);
			mesh.indices.push_back(current_vertex + resolution + 1);
			mesh.indices.push_back(current_vertex + resolution);
		}
	}
}

void Cloth::setRenderer(MeshRenderer* renderer)
{
	meshRenderer = renderer;
	meshRenderer->position = position;
}

void Cloth::MeshUpdateAll()
{
	for (int i = 0; i < ClothObjects.size(); i++)
		ClothObjects[i]->meshUpdate();
}

void Cloth::meshUpdate()
{
	if (meshRenderer) meshRenderer->updateMesh(mesh);
	else std::cout << "ERROR :: MESH RENDERER NOT ATTACHED TO CLOTH AT (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}

void Cloth::fixedUpdate()
{
	collider.GenerateParticleHash(mesh.vertices);
	// Apply simulation step to every vertex
	// TODO: substepping?
	for (int idx = 0; idx < numParticles; idx++)
	{
		// TODO: Limit stretching to 10%
		SpringMassSimulationStep(idx);

		UpdateNormals(idx);

		HandleCollisions(idx);
	}
}

void Cloth::UpdateNormals(int idx)
{
	glm::vec3 normal{};
	glm::vec3 pos{ vertices[idx].position };
	int i = idx / resolution;
	int j = idx % resolution;
	int indices[4][2]{ {-1, 0}, {0, -1}, {1, 0}, {0, 1} };
	for (int k = 0; k < 4; k++)
	{
		int* idx{ indices[k] };
		int* newIdx{ indices[(k + 1) % 4] };

		int x{ i + idx[0] };
		int y{ j + idx[1] };
		int x2{ i + newIdx[0] };
		int y2{ j + newIdx[1] };
		int p1 = x * resolution + y;
		int p2 = x2 * resolution + y2;
		if (x < 0 || x >= resolution || y < 0 || y >= resolution || x2 < 0 || x2 >= resolution || y2 < 0 || y2 >= resolution) continue;
		normal += glm::cross(vertices[p2].position - pos, vertices[p1].position - pos);
	}
	// Update normal in mesh
	normal = glm::normalize(normal);
	mesh.normals[3 * idx] = normal.x;
	mesh.normals[3 * idx + 1] = normal.y;
	mesh.normals[3 * idx + 2] = normal.z;
}

void Cloth::HandleCollisions(int idx)
{
	std::vector<int> collisions{ collider.DetectCollisions(mesh.vertices, idx) };

	Vertex vert{ vertices[idx] };
	for (int collidedIdx : collisions)
	{
		if (adjacent(idx, collidedIdx)) continue;
		Vertex v2{ vertices[collidedIdx] };

		glm::vec3 disp{ v2.position - vert.position };
		float dist{ glm::length(disp) };
		if (dist == 0 || dist > clothThickness) continue;

		// position correction
		glm::vec3 offset{ (clothThickness - dist) * 0.5F * glm::normalize(disp) };
		vert.position -= offset;
		v2.position += offset;

		//// Friction method for velocity correction
		//glm::vec3 velAvg{ (vert.position - vert.lastPosition + v2.position - v2.lastPosition) * 0.5F };
		//vert.position += 0.1F * velAvg; // 0.1 is arbitrary damping coefficient [0,1]
		//v2.position += 0.1F * velAvg;

		// Update vertices
		vertices[idx] = vert;
		vertices[collidedIdx] = v2;

		// Update mesh vertices
		mesh.vertices[3 * idx] = vert.position.x;
		mesh.vertices[3 * idx + 1] = vert.position.y;
		mesh.vertices[3 * idx + 2] = vert.position.z;
	}
}

void Cloth::SpringMassSimulationStep(int idx)
{
	// assuming mass = 1
	Vertex vert = vertices[idx];
	int i = idx / resolution;
	int j = idx % resolution;
	// Accumulate spring force from nearby vertices
	for (int a = 0; a <= 1; a++)
		for (int b = -1; b <= 1; b++)
		{
			// Shear and Structural springs
			if (a == 0 && b <= 0) continue;
			int x = i + a;
			int y = j + b;
			int newIdx = x * resolution + y;
			if (x < 0 || x >= resolution || y < 0 || y >= resolution) continue;
			//  X X X
			//  X X 1		<-	a + b
			//  0 1 2
			float rest{ abs(a + b) == 1 ? restLen : shearRestLen };
			// Take vertex position before moving this frame
			glm::vec3 vertPos{ a + b <= 0 && a < 1 ? vertices[newIdx].lastPosition : vertices[newIdx].position };

			glm::vec3 distance = vertPos - vert.position;
			glm::vec3 force{ springConstant * (glm::length(distance) - rest) * glm::normalize(distance) };
			vert.springForce += force;
			vertices[newIdx].springForce -= force;

			// Damping force (Müller et al.)
			glm::vec3 damping{ springDamping * glm::dot(vertices[newIdx].velocity - vert.velocity, distance) / glm::dot(distance, distance) * distance };
			vert.springForce += damping;
			vertices[newIdx].springForce -= damping;

			// Bending springs
			int x2 = i + 2 * a;
			int y2 = j + 2 * b;
			newIdx = x2 * resolution + y2;
			if (x2 < 0 || x2 >= resolution || y2 < 0 || y2 >= resolution) continue;
			distance = vertices[newIdx].position - vert.position;
			force = bendingSpringConstant * (glm::length(distance) - 2 * rest) * glm::normalize(distance);
			vert.springForce += force;
			vertices[newIdx].springForce -= force;

			/*damping = springDamping * glm::dot(vertices[newIdx].velocity - vert.velocity, distance) / glm::dot(distance, distance) * distance;
			vert.springForce += damping;
			vertices[newIdx].springForce -= damping;*/
		}
	// Leave anchors alone
	if ((i == anchors[0] && j == anchors[1]) || (i == anchors[2] && j == anchors[3])) return;

	// TODO: add wind, etc.
	glm::vec3 vel{ (vert.position - vert.lastPosition) / Physics::FIXEDDELTATIME };
	vertices[idx].velocity = vel;
	float speed = glm::length(vel);
	glm::vec3 airResistance{};
	if (speed != 0) airResistance = airResistivity * speed * speed * -glm::normalize(vel);
	glm::vec3 Anet = Physics::GRAVITY + airResistance + vert.springForce;
	vertices[idx].springForce = {};

	// Verlet Integration
	glm::vec3 newPosition = 2.0F * vert.position - vert.lastPosition + Anet * Physics::FIXEDDELTATIME * Physics::FIXEDDELTATIME;
	vertices[idx].lastPosition = vert.position;
	vertices[idx].position = newPosition;

	// Update mesh vertices
	mesh.vertices[3 * idx] = vert.position.x;
	mesh.vertices[3 * idx + 1] = vert.position.y;
	mesh.vertices[3 * idx + 2] = vert.position.z;

	// Update Shader visualization
	glm::vec3 change{ vert.position - vert.lastPosition };
	mesh.attributes[1][3 * idx] = change.x;
	mesh.attributes[1][3 * idx + 1] = change.y;
	mesh.attributes[1][3 * idx + 2] = change.z;
}