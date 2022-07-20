#pragma once
#include "Transform.h"

class Object
{
public:
	Transform transform;
	virtual void Draw(const XMMATRIX& viewProjectionMatrix) {};
};