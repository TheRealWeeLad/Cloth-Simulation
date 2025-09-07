#include "Component.h"
#include "GameObject.h"

using namespace Engine;

void Component::linkObject(GameObject* obj)
{
	gameObject = obj;
}