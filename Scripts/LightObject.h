#pragma once
#include "Shader.h"
#include <vector>
#include <glm/glm.hpp>

class LightObject
{
public:
    static std::vector<LightObject*> LightObjects;
    static LightMaterial LightMat;
    static glm::vec3 LightPos;
    LightMaterial mat;
    glm::vec3 position;

    LightObject(LightMaterial lightMat);
    LightObject(LightMaterial lightMat, glm::vec3 position);
    ~LightObject();

    static void CalculateLighting();
};