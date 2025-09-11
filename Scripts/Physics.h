#pragma once
#include <glm/glm.hpp>
#include <vector>

class Cloth;

static class Physics
{
private:
	static std::vector<Cloth*> _clothObjects;
public:
	static const glm::vec3 GRAVITY;
	static const float FIXEDDELTATIME;

	static void AddObject(Cloth* cloth);
	static void FixedUpdateAll();
};