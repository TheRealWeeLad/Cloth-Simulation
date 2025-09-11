#pragma once
#include <glm/glm.hpp>
#include <vector>

static class Physics
{
private:
public:
	static const glm::vec3 GRAVITY;
	static const float FIXEDDELTATIME;

	static void FixedUpdateAll();
};