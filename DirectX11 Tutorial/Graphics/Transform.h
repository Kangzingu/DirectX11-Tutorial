#pragma once
#include "Model.h"

class Transform
{
public:
	Matrix4x4 worldMatrix;
	void UpdateMatrix();

	Vector3 position;
	Vector3 rotation;
	Vector3 scale;

	Vector3 right;
	Vector3 up;
	Vector3 forward;
};