#include "Camera.h"

#include "../Utils/SimpleMath.h"

bool Camera::Initialize()
{
	this->transform.position = { 0, 0, 0 };
	this->transform.rotation = { 0, 0, 0 };
	this->transform.UpdateMatrix();
	return true;
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	//this->projectionMatrix = Matrix4x4(XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ)).ToXMMATRIX();
	this->projectionMatrix = (Matrix4x4::Projection(Radian2Degree(fovRadians), aspectRatio, nearZ, farZ)).ToXMMATRIX();
}

void Camera::UpdateMatrix()
{
	this->transform.UpdateMatrix();
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->transform.rotation.x, this->transform.rotation.y, this->transform.rotation.z);
	XMVECTOR camTarget = XMVector3TransformCoord((this->transform.worldMatrix * Vector3::Forward()).ToXMVECTOR(), camRotationMatrix);
	camTarget += this->transform.posVector;
	XMVECTOR upDir = XMVector3TransformCoord(this->transform.DEFAULT_UP_VECTOR, camRotationMatrix);
	this->viewMatrix = XMMatrixLookAtLH(this->transform.posVector, camTarget, upDir);
	
	this->transform.UpdateDirectionVectors();
}