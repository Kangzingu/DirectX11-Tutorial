#pragma once
#include "../Utils/SimpleMath.h"

class Collider
{
public:
	Collider() {}
	void Initialize();

	bool isEnabled = true;
	Vector3 center = Vector3::Zero();
};

class SphereCollider : public Collider
{
public:
	SphereCollider() {}
	float radius = 1.0f;
};

class CubeCollider : public Collider
{
public:
	CubeCollider() {}
	Vector3 size = Vector3::One();
};

class PlaneCollider : public Collider
{
public:
	PlaneCollider() {}
	Vector3 normal = Vector3::Forward();
	float offset = 0;
};