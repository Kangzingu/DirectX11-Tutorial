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
	float m_resolveSpeed;
	float m_restitution =0.1f;
	Matrix4x4 m_contactToWorld;
	Vector3 m_ContactVelocity;
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
	void ResolveVelocity();
	void ResolvePosition();
};