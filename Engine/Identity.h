#ifndef IDENTITY
#define IDENTITY
#include <glm/gtc/quaternion.hpp>

namespace Engine {
static const glm::quat IDENTITY_QUAT{ glm::quat(0, 0, 0, 1) };
}
#endif