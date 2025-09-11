#include "Physics.h"
#include "Cloth.h"

const glm::vec3 Physics::GRAVITY{ 0, 2, 0 };
const float Physics::FIXEDDELTATIME{ 0.01F };

void Physics::FixedUpdateAll()
{
	for (int i = 0; i < Cloth::ClothObjects.size(); i++)
	{
		Cloth::ClothObjects[i]->fixedUpdate();
	}
}