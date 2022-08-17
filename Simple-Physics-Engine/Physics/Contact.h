#pragma once
#include "../Utils/SimpleMath.h"

class Object;
class Contact
{
public:
	Object* m_objects[2];
	Vector3 m_point;
	Vector3 m_normal;
	float m_penetration;
	float m_friction = 0.7f;
	float m_restitution = 0.5f;
	Matrix4x4 m_contactToWorld;
	Vector3 m_separateVelocity;
	Vector3 m_relativeContactPosition[2];

	float m_desiredDeltaVelocity;

public:
	void SwapObjects();
	void MatchAwakeState();

	void CalculateInternals(float deltaTime);
	void CalculateContactToWorldMatrix();
	void CalculateRelativeContactPosition();

	Vector3 CalculateFrictionlessImpulse();
	Vector3 CalculateFrictionImpulse();
	Vector3 CalculateLocalVelocity(int index, float deltaTime);
	void CalculateLocalContactVelocity(float deltaTime);
	void CalculateDesiredDeltaVelocity(float deltaTime);
	void ChangeVelocity(Vector3 velocityChange[2], Vector3 angularVelocityChange[2]);
	void ResolvePenetration(Vector3 linearChange[2], Vector3 angularChange[2], float penetration);
};