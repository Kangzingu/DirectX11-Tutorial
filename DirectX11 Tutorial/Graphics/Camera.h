#pragma once
#include "Object.h"
using namespace DirectX;

class Camera : public Object
{
public:
	Camera();
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	const XMMATRIX& GetViewMatrix() const;
	const XMMATRIX& GetProjectionMatrix() const;
	void UpdateMatrix();
private:

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
};