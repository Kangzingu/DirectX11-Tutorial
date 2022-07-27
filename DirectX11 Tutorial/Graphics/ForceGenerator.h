#pragma once
#include <DirectXMath.h>
#include "Rigidbody.h"
using namespace DirectX;

class Gravity
{
public:
	Vector3 force = Vector3(0, -9.8f, 0);
};

class ForceGenerator
{
public:
	Gravity gravity;
	void GenerateGravity(Rigidbody& rigidbody);
};
