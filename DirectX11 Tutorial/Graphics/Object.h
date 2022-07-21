#pragma once
#include "Transform.h"
#include "Rigidbody.h"

class Object
{
public:
	Object() {}
	Transform transform;
	virtual void Draw(Matrix4x4 viewProjectionMatrix) {};
};