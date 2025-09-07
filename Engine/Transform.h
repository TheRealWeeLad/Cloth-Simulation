#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Identity.h"

namespace Engine {

class Component;

class Transform
{
public:
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	Transform();
	Transform(glm::vec3 position, glm::quat rotation = IDENTITY_QUAT, glm::vec3 scale = {});

	void update();
	void translate(glm::vec3 translation);
};
}