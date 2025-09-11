#include "ClothCollider.h"
#include "Debug.cpp"

using namespace Engine;

/* PRIVATE */
// Thanks to Matthias Müller
int ClothCollider::hash(int x, int y, int z) const
{
	return abs((x * 92837111) ^ (y * 689287499) ^ (z * 283923481)) % TABLE_SIZE;
}

/* PUBLIC */
ClothCollider::ClothCollider(int numParticles, float spacing, ClothCollisionMethod method) :
	method{ method }, numParticles{ numParticles }, TABLE_SIZE { 2 * numParticles }, 
	spacing{ spacing }, hashTable(TABLE_SIZE + 1), hashParticles(numParticles)
	{ }

void ClothCollider::GenerateParticleHash(std::vector<float> vertices)
{
	// reset
	std::fill(hashTable.begin(), hashTable.end(), 0);
	std::fill(hashParticles.begin(), hashParticles.end(), 0);

	// Determine number of particles in each cell
	for (int i = 0; i < numParticles; i++)
	{
		int x{ (int)(vertices[3 * i] / spacing) };
		int y{ (int)(vertices[3 * i + 1] / spacing) };
		int z{ (int)(vertices[3 * i + 2] / spacing) };
		int h{ hash(x, y, z) };
		hashTable[h]++;
	}

	// Calculate partial sums
	int start{};
	for (int i = 0; i < TABLE_SIZE; i++)
	{
		start += hashTable[i];
		hashTable[i] = start;
	}
	hashTable[TABLE_SIZE] = start; // prevent overflow when querying

	// Fill indices into particles list
	for (int i = 0; i < numParticles; i++)
	{
		int x{ (int)(vertices[3 * i] / spacing) };
		int y{ (int)(vertices[3 * i + 1] / spacing) };
		int z{ (int)(vertices[3 * i + 2] / spacing) };
		int h{ hash(x, y, z) };
		hashTable[h]--;
		hashParticles[hashTable[h]] = i;
	}
}

std::vector<int> ClothCollider::DetectCollisions(std::vector<float> vertices, int idx)
{
	std::vector<int> collisions{};
	switch (method)
	{
		case ParticleHash:
		{
			// Query a block of cells to check for collisions
			int x0{ (int)(vertices[3 * idx] / spacing - 1) };
			int y0{ (int)(vertices[3 * idx + 1] / spacing - 1) };
			int z0{ (int)(vertices[3 * idx + 2] / spacing - 1) };

			int x1{ (int)(vertices[3 * idx] / spacing + 1) };
			int y1{ (int)(vertices[3 * idx + 1] / spacing + 1) };
			int z1{ (int)(vertices[3 * idx + 2] / spacing + 1) };

			for (int xi = x0; xi <= x1; xi++)
				for (int yi = y0; yi <= y1; yi++)
					for (int zi = z0; zi <= z1; zi++)
					{
						int h{ hash(xi, yi, zi) };
						int start{ hashTable[h] };
						int end{ hashTable[h + 1] };

						for (int j = start; j < end; j++)
							collisions.push_back(hashParticles[j]);
					}

			break;
		}
		case BoundingBox:
			break;
	}
	return collisions;
}