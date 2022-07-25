#include "Camera.h"

#include "../Utils/SimpleMath.h"

void Camera::SetProjectionMatrix(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	this->projectionMatrix = Matrix4x4::Projection(fovDegrees, aspectRatio, nearZ, farZ);
	this->UpdateMatrix();
}
void Camera::UpdateMatrix()
{
	Vector3 eyePosition = this->transform.GetPosition();
	Vector3 targetPosition = this->transform.GetPosition() + this->transform.GetForward();
	Vector3 upDirection = this->transform.GetUp();
	this->viewMatrix = Matrix4x4(XMMatrixLookAtLH(eyePosition.ToXMVECTOR(), targetPosition.ToXMVECTOR(), upDirection.ToXMVECTOR()));
}