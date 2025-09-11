#include "LightObject.h"

using namespace Engine;

std::vector<LightObject*> LightObject::LightObjects{};
LightMaterial LightObject::LightMat{ };
glm::vec3 LightObject::LightPos{ 0, 0, 0 };


LightObject::LightObject(LightMaterial lightMat) : GameObject()
{
	mat = lightMat;
	LightObjects.push_back(this);
}
LightObject::LightObject(LightMaterial lightMat, glm::vec3 position) : GameObject(position)
{
	mat = lightMat;
	LightObjects.push_back(this);
}
LightObject::~LightObject()
{
	LightObjects.erase(std::find(LightObjects.begin(), LightObjects.end(), this));
}

void LightObject::CalculateLighting()
{
	// Reset color and position every frame
	LightMat = {};
	LightPos = { 0, 0, 0 };
	for (int i = 0; i < LightObjects.size(); i++)
	{
		LightObject* l{ LightObjects[i] };
		LightMat *= l->mat;
		LightPos += LightObjects[i]->transform.position;
	}
	LightPos /= LightObjects.size();
}