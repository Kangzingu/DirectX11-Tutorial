#pragma once
#include "../Utils/SimpleMath.h"
#include "Collision.h"
#include <vector>

class Actor;
class PhysicsManager
{
private:
	bool isGravityOn = true;
	Vector3 gravity = Vector3(0, -9.8f, 0);

public:
	void Update(vector<Actor*> actors, float deltaTime);

public:
};