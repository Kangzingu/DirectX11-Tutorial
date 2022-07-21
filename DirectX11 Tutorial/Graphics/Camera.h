#pragma once
#include "Object.h"
using namespace DirectX;

class Camera : public Object
{
public:
	bool Initialize();
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	void UpdateMatrix();
private:

	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
};