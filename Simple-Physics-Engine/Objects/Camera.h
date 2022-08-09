#pragma once
#include "Object.h"

class Camera : public Object
{
public:
	Camera() {}
	void SetProjectionMatrix(float fovDegrees, float aspectRatio, float nearZ, float farZ);
	void UpdateMatrix();

	void SetViewMatrix(Matrix4x4 viewMatrix);
	void SetProjectionMatrix(Matrix4x4 projectionMatrix);

	Matrix4x4& GetViewMatrix();
	Matrix4x4& GetProjectionMatrix();
	Matrix4x4& GetViewProjectionMatrix();

private:
	Matrix4x4 m_viewMatrix;
	Matrix4x4 m_projectionMatrix;
	Matrix4x4 m_viewProjectionMatrix;
};