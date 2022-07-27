#pragma once
#include "../Utils/SimpleMath.h"

class Object;
class Contact
{
public:
	Object* object1;
	Object* object2;
	Vector3 point;
	Vector3 normal;
	float penetration;
};