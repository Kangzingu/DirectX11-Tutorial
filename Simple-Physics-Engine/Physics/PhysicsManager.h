#pragma once
#include "../Utils/SimpleMath.h"
#include "Collision.h"
#include <vector>

class Actor;
class PhysicsManager
{
private:
	const float m_epsilon = 0.01f;
	Vector3 m_gravity = Vector3(0, -9.8f, 0);
	vector<Contact> m_contacts;

	vector<Actor*>* m_actors;
	float& m_deltaTime;
	vector<Vector3>* m_lineForDebug;

public:
	PhysicsManager(vector<Actor*>* actors, float& deltaTime, vector<Vector3>* lineForDebug);

	void Update();
	void GenerateGeneralForces();
	void UpdateTransform();
	void DetectCollision();
	void ResolveCollision();
	void CalculateInternals();
	void ResolvePenetration();
	void ResolveVelocity();
};