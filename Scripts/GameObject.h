#pragma once
#include "MeshRenderer.h"
#include "Transform.h"
#include <glm/glm.hpp>
#include <array>

namespace Engine {

// Forward declare includes to avoid circular dependencies
class Component;

class GameObject
{
private:
	static const int MAX_NUM_COMPONENTS = 100;
public:
	static std::vector<GameObject*> GameObjects;
	// Array of components each located at index COMPONENT::ID
	std::array<Component*, MAX_NUM_COMPONENTS> components;

	Transform transform;

	GameObject();
	GameObject(glm::vec3 position);
	~GameObject();

	virtual void update(float deltaTime);
	virtual void fixedUpdate();
	static void UpdateAll(float deltaTime);
	static void FixedUpdateAll();

	template<typename SomeComponent>
	void addComponent(SomeComponent* c)
	{
		components[c->ID] = c;
		c->linkObject(this);
	}

	template<typename SomeComponent>
	SomeComponent* getComponent()
	{
		return (SomeComponent*) components[SomeComponent::ID];
	}

	template<typename SomeComponent>
	void removeComponent()
	{
		components[SomeComponent::ID] = nullptr;
	}
};
}