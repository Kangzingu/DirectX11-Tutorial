#include "Camera.h"
#include "../Utils/SimpleMath.h"

void Camera::SetProjectionMatrix(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	m_nearZ = nearZ;
	m_farZ = farZ;
	m_projectionMatrix = Matrix4x4::Projection(fovDegrees, aspectRatio, m_nearZ, m_farZ);
	UpdateMatrix();
}
float Camera::GetNearZ()
{
	return m_nearZ;
}
float Camera::GetFarZ()
{
	return m_farZ;
}
void Camera::SetSpeed(float speed)
{
	m_speed = speed;
}
void Camera::SetBackgroundColor(float* backgroundColor)
{
	for (int i = 0; i < 4; i++)
	{
		m_backgroundColor[i] = backgroundColor[i];
	}
}
void Camera::UpdateMatrix()
{
	m_viewMatrix = m_transform.GetWorldMatrix().Inverse();
	m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
}
void Camera::SetViewMatrix(Matrix4x4 viewMatrix)
{
	m_viewMatrix = viewMatrix;
}
void Camera::SetProjectionMatrix(Matrix4x4 projectionMatrix)
{
	m_projectionMatrix = projectionMatrix;
	UpdateMatrix();
}
Matrix4x4& Camera::GetViewMatrix()
{
	return m_viewMatrix;
}
Matrix4x4& Camera::GetProjectionMatrix()
{
	return m_projectionMatrix;
}
Matrix4x4& Camera::GetViewProjectionMatrix()
{
	return m_viewProjectionMatrix;
}

float Camera::GetSpeed()
{
	return m_speed;
}

float* Camera::GetBackgroundColor()
{
	return m_backgroundColor;
}
