#pragma once

namespace Engine {

class GameObject;

class Component
{
public:
	GameObject* gameObject;
	const static unsigned short ID{ 0 }; // To identify components

	virtual void update() {};
	void linkObject(GameObject* obj);
};
}