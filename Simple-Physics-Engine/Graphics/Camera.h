#pragma once
#include "Object.h"

class Camera : public Object
{
public:
	Camera() {}
	void SetProjectionMatrix(float fovDegrees, float aspectRatio, float nearZ, float farZ);
	void UpdateMatrix();

	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
};