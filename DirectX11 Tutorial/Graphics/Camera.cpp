#include "Camera.h"

#include "../Utils/SimpleMath.h"

bool Camera::Initialize()
{
	this->transform.position = { 0, 5, 0 };
	this->transform.rotation = { 0, 50, 30 };
	this->transform.UpdateMatrix();
	this->UpdateMatrix();
	return true;
}

void Camera::SetProjectionMatrix(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	this->projectionMatrix = Matrix4x4::Projection(fovDegrees, aspectRatio, nearZ, farZ);
}

void Camera::UpdateMatrix()
{
	this->transform.UpdateMatrix();
	Vector3 eyePosition = this->transform.position;
	Vector3 targetPosition = this->transform.position + this->transform.forward;
	Vector3 upDirection = this->transform.up;
	this->viewMatrix = Matrix4x4(XMMatrixLookAtLH(eyePosition.ToXMVECTOR(), targetPosition.ToXMVECTOR(), upDirection.ToXMVECTOR()));
}