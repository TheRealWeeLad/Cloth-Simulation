#ifndef CameraClass
#define CameraClass

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Identity.h"

class Camera
{
public:
	static Camera* MainCamera;

	enum ViewMode { TARGET, FREE };
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;
	glm::mat4 view;
	glm::quat rotation;
	float fov;

	Camera(glm::vec3 position, glm::quat rotation = IDENTITY_QUAT, float fov = 45.0f, ViewMode mode = ViewMode::FREE, glm::vec3 target = {});
	Camera(glm::vec3 position, glm::vec3 eulerAngles, float fov = 45.0f, ViewMode mode = ViewMode::FREE, glm::vec3 target = {});
	Camera();

	void setPosition(glm::vec3 position);
	void translate(glm::vec3 translation);
	void rotate(glm::vec3 eulerAngles);
	void rotate(glm::quat quaternion);

	void beginTargeting(glm::vec3 target);
	void stopTargeting();

	void calculateViewMatrix();
private:
	ViewMode mode;

	void calculateDirections();
};
#endif