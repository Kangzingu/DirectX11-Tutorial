#pragma once
#include "Object.h"

class Camera : public Object
{
public:
	Camera() {}

	void SetSpeed(float speed);
	void SetBackgroundColor(float* backgroundColor);

	void UpdateMatrix();
	void SetViewMatrix(Matrix4x4 viewMatrix);
	void SetProjectionMatrix(Matrix4x4 projectionMatrix);
	void SetProjectionMatrix(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	float GetNearZ();
	float GetFarZ();
	float GetSpeed();
	float* GetBackgroundColor();

	Matrix4x4& GetViewMatrix();
	Matrix4x4& GetProjectionMatrix();
	Matrix4x4& GetViewProjectionMatrix();

private:
	float m_nearZ;
	float m_farZ;
	float m_speed = 10.0f;
	float m_backgroundColor[4] = { 0, 0, 0, 1 };

	Matrix4x4 m_viewMatrix;
	Matrix4x4 m_projectionMatrix;
	Matrix4x4 m_viewProjectionMatrix;
};