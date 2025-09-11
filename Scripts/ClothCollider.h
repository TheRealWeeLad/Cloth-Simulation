#pragma once
#include <vector>

enum ClothCollisionMethod { ParticleHash, BoundingBox };

class ClothCollider
{
private:
	const int TABLE_SIZE;
	const int numParticles;
	const float spacing;
	std::vector<int> hashTable;
	std::vector<int> hashParticles;
	int hash(int x, int y, int z) const;
public:
	ClothCollisionMethod method;

	ClothCollider(int numParticles, float spacing, ClothCollisionMethod method = ParticleHash);

	void GenerateParticleHash(std::vector<float> vertices);
	std::vector<int> DetectCollisions(std::vector<float> vertices, int idx);
};