#include "Camera.h"

Camera cam{};
Camera* Camera::MainCamera{ &cam };
const glm::vec3 Camera::forward{0, 0, -1};
const glm::vec3 Camera::up{0, 1, 0};
const glm::vec3 Camera::right{1, 0, 0};

/* CONSTRUCTORS */
Camera::Camera(glm::vec3 position, glm::quat rotation, float fov, ViewMode mode, glm::vec3 target)
{
	this->position = position;
	this->rotation = rotation;
	this->fov = fov;
	this->mode = mode;
	this->target = target;

	calculateViewMatrix();
}
Camera::Camera(glm::vec3 position, glm::vec3 eulerAngles, float fov, ViewMode mode, glm::vec3 target)
	: Camera(position, glm::quat(eulerAngles), fov, mode, target) {}
Camera::Camera() : Camera(glm::vec3(0, 0, 3)) {}

Camera::ViewMode Camera::getMode() const
{
	return mode;
}
void Camera::setPosition(glm::vec3 position)
{
	this->position = position;
	calculateViewMatrix();
}
void Camera::translate(glm::vec3 translation)
{
	position += rotation * translation;
	calculateViewMatrix();
}
void Camera::rotate(glm::quat quaternion)
{
	if (mode == TARGET) return;
	rotation = quaternion * rotation;
	calculateViewMatrix();
}
void Camera::rotate(glm::vec3 eulerAngles)
{
	rotate(glm::quat(eulerAngles));
}

void Camera::beginTargeting(glm::vec3 target)
{
	mode = ViewMode::TARGET;
	this->target = target;
	calculateViewMatrix();
}
void Camera::stopTargeting()
{
	mode = ViewMode::FREE;
	calculateViewMatrix();
}

void Camera::calculateViewMatrix()
{
	if (mode == TARGET) rotation = glm::quatLookAt(glm::normalize(target - position), rotation * up);
	view = glm::lookAt(position, position + rotation * forward, rotation * up);
}