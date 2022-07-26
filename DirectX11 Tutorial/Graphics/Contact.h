#pragma once
#include "../Utils/SimpleMath.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "Collider.h"

class Contact
{
public:
	Transform* t1;
	Transform* t2;
	Vector3 point;
	Vector3 normal;
	float penetration;
};