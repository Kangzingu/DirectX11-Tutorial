#pragma once
#include "Object.h"
using namespace DirectX;

class Camera : public Object
{
public:
	void Initialize(float fovDegrees, float aspectRatio, float nearZ, float farZ);
	void SetProjectionMatrix(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	void UpdateMatrix();

	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
};