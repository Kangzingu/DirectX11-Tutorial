#pragma once
#include "../Utils/SimpleMath.h"
#include "Collision.h"
#include <vector>

class Actor;
class PhysicsManager
{
private:
	const float epsilon = 0.00001f;
	Vector3 gravity = Vector3(0, -9.8f, 0);
	vector<Contact> contacts;

	vector<Actor*>* actors;
	float& deltaTime;
	vector<Vector3>* lineForDebug;

public:
	PhysicsManager(vector<Actor*>* actors, float& deltaTime, vector<Vector3>* lineForDebug);

	void Update();
	void GenerateGravity();
	void UpdateTransform();
	void DetectCollision();
	void ResolveCollision();

public:
};