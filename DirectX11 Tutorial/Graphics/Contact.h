#pragma once
#include "../Utils/SimpleMath.h"

class Object;
class Contact
{
public:
	Object* objects[2];
	Vector3 point;
	Vector3 normal;
	float depth;
	float resolveSpeed;
	float restitution = 0.4f;
	Matrix4x4 contactToWorld;
	Vector3 localContactVelocity;
	Vector3 relativeContactPosition[2];

public:
	void CalculateInternals(float deltaTime);
	void CalculateContactToWorldMatrix();
	void CalculateRelativeContactPosition();
	void CalculateLocalContactVelocity(float deltaTime);
	void CalculateResolveSpeed(float deltaTime);
};