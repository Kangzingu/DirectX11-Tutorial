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
	float m_restitution =0.01f;
	Matrix4x4 m_contactToWorld;
	Vector3 m_contactVelocity;
	Vector3 m_relativeContactPosition[2];

	float m_desiredDeltaVelocity;

public:
	void CalculateInternals(float deltaTime);
	void CalculateContactToWorldMatrix();
	void CalculateRelativeContactPosition();
	//void CalculateLocalContactVelocity(float deltaTime);
	//void CalculateResolveSpeed(float deltaTime);
	
	Vector3 CalculateImpulse();
	Vector3 CalculateLocalVelocity(int index, float deltaTime);
	void CalculateLocalContactVelocity(float deltaTime);
	void CalculateDesiredDeltaVelocity(float deltaTime);
	void ModifyVelocity(Vector3 velocityChange[2], Vector3 angularVelocityChange[2]);
	void ModifyPosition(Vector3 linearChange[2], Vector3 angularChange[2], float penetration);
};