#pragma once
#include "Transform.h"
#include "Rigidbody.h"

class Object
{
public:
	Object() {}
	Transform transform;
	virtual void Draw(const XMMATRIX& viewProjectionMatrix) {};
};