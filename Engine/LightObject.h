#pragma once
#include "GameObject.h"

namespace Engine {

class LightObject :
    public GameObject
{
public:
    static std::vector<LightObject*> LightObjects;
    static LightMaterial LightMat;
    static glm::vec3 LightPos;
    LightMaterial mat;

    LightObject(LightMaterial lightMat);
    LightObject(LightMaterial lightMat, glm::vec3 position);
    ~LightObject();

    static void CalculateLighting();
};
}
