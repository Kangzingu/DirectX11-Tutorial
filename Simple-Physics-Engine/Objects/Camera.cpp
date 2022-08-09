#include "Camera.h"
#include "../Utils/SimpleMath.h"

void Camera::SetProjectionMatrix(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	m_projectionMatrix = Matrix4x4::Projection(fovDegrees, aspectRatio, nearZ, farZ);
	UpdateMatrix();
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