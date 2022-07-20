#include "Camera.h"

bool Camera::Initialize()
{
	this->transform.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->transform.posVector = XMLoadFloat3(&this->transform.pos);
	this->transform.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->transform.rotVector = XMLoadFloat3(&this->transform.rot);
	this->transform.UpdateMatrix();
	return true;
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

const XMMATRIX& Camera::GetViewMatrix() const
{
	return this->viewMatrix;
}

const XMMATRIX& Camera::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}

void Camera::UpdateMatrix()
{
	this->transform.UpdateMatrix();
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->transform.rot.x, this->transform.rot.y, this->transform.rot.z);
	XMVECTOR camTarget = XMVector3TransformCoord(this->transform.DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	camTarget += this->transform.posVector;
	XMVECTOR upDir = XMVector3TransformCoord(this->transform.DEFAULT_UP_VECTOR, camRotationMatrix);
	this->viewMatrix = XMMatrixLookAtLH(this->transform.posVector, camTarget, upDir);
	
	this->transform.UpdateDirectionVectors();
}