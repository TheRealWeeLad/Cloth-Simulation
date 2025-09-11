#include "Camera.h"

Camera cam{};
Camera* Camera::MainCamera{ &cam };

/* CONSTRUCTORS */
Camera::Camera(glm::vec3 position, glm::quat rotation, float fov, ViewMode mode, glm::vec3 target)
{
	this->position = position;
	this->rotation = rotation;
	this->fov = fov;
	this->mode = mode;
	this->target = target;

	calculateDirections();
	calculateViewMatrix();
}
Camera::Camera(glm::vec3 position, glm::vec3 eulerAngles, float fov, ViewMode mode, glm::vec3 target)
	: Camera(position, glm::quat(eulerAngles), fov, mode, target) {}
Camera::Camera() : Camera(glm::vec3(0, 0, 3)) {}

void Camera::setPosition(glm::vec3 position)
{
	this->position = position;
	calculateViewMatrix();
}
void Camera::translate(glm::vec3 translation)
{
	position += translation;
	calculateViewMatrix();
}
void Camera::rotate(glm::quat quaternion)
{
	rotation = quaternion * rotation;
	calculateDirections();
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

void Camera::calculateDirections()
{
	forward = glm::vec3(0, 0, -1) * rotation;
	up = glm::vec3(0, 1, 0) * rotation;
	right = glm::vec3(1, 0, 0) * rotation;
}

void Camera::calculateViewMatrix()
{
	glm::vec3 targetDir;
	switch (mode)
	{
	case TARGET:
		targetDir = target;
		break;
	case FREE:
		targetDir = position + forward;
		break;
	}

	view = glm::lookAt(position, targetDir, up);
}