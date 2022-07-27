#pragma once
#include "Object.h"

class Actor : public Object
{
public:
	Actor() {}
	void Draw(Matrix4x4& viewProjectionMatrix) override;
};