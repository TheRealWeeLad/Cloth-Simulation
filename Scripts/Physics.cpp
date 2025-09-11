#include "Physics.h"
#include "Cloth.h"

const glm::vec3 Physics::GRAVITY{ 0, 2, 0 };
const float Physics::FIXEDDELTATIME{ 0.01F };

void Physics::AddObject(Cloth* cloth)
{
	_clothObjects.push_back(cloth);
}

void Physics::FixedUpdateAll()
{
	for (int i = 0; i < _clothObjects.size(); i++)
	{
		_clothObjects[i]->fixedUpdate();
	}
}