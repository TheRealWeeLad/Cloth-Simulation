#include "Transform.h"

using namespace Engine;

/* CONSTRUCTOR */
Transform::Transform() : Transform(glm::vec3{}) {}
Transform::Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale) :
	position{position}, rotation{rotation}, scale{scale} { }

/* METHODS */
void Transform::update()
{
	// TODO
}

void Transform::translate(glm::vec3 translation)
{
	position += translation;
}