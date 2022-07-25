#pragma once
#include "Object.h"

class Actor : public Object
{
public:
	void Draw(Matrix4x4& viewProjectionMatrix) override;
};