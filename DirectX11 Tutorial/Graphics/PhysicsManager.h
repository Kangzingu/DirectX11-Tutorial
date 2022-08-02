#pragma once
#include "../Utils/SimpleMath.h"
#include "Collision.h"
#include <vector>

class Actor;
class PhysicsManager
{
private:
	bool isGravityOn = false;
	Vector3 gravity = Vector3(0, -3.8f, 0);

public:
	void Update(vector<Actor*> actors, float deltaTime, vector<pair<Vector3, Vector3>>& aabb);
	void UpdateTransform(Rigidbody* rigidbody, float deltaTime);

	void SetGravityOn(bool isGravityOn);
public:
};