#pragma once
#include <DirectXMath.h>
#include "Rigidbody.h"
using namespace DirectX;

class Gravity
{
public:
	XMVECTOR force = XMVectorSet(0, -9.8f, 0, 0);
};

class ForceGenerator
{
public:
	Gravity gravity;
	void GenerateGravity(Rigidbody& rigidbody);
};
