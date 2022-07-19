#include "Transform.h"

const XMVECTOR& Transform::GetPositionVector() const
{
	return this->posVector;
}
const XMFLOAT3& Transform::GetPositionFloat3() const
{
	return this->pos;
}
const XMVECTOR& Transform::GetRotationVector() const
{
	return this->rotVector;
}
const XMFLOAT3& Transform::GetRotationFloat3() const
{
	return this->rot;
}
const XMVECTOR& Transform::GetScaleVector() const
{
	return this->scaleVector;
}
const XMFLOAT3& Transform::GetScaleFloat3() const
{
	return this->scale;
}
void Transform::SetPosition(const XMVECTOR& pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->UpdateMatrix();
}
void Transform::SetPosition(const XMFLOAT3& pos)
{
	this->pos = pos;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}
void Transform::SetPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}
void Transform::AdjustPosition(const XMVECTOR& pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateMatrix();
}
void Transform::AdjustPosition(const XMFLOAT3& pos)
{
	this->pos.x += pos.x;
	this->pos.y += pos.y;
	this->pos.z += pos.z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}
void Transform::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}
void Transform::SetRotation(const XMVECTOR& rot)
{
	this->rotVector = rot;
	XMStoreFloat3(&this->rot, rot);
	this->UpdateMatrix();
}
void Transform::SetRotation(const XMFLOAT3& rot)
{
	this->rot = rot;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}
void Transform::SetRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}
void Transform::AdjustRotation(const XMVECTOR& rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateMatrix();
}
void Transform::AdjustRotation(const XMFLOAT3& rot)
{
	this->rot.x += rot.x;
	this->rot.y += rot.y;
	this->rot.z += rot.z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}
void Transform::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}
void Transform::SetScale(const XMVECTOR& scale)
{
	XMStoreFloat3(&this->scale, scale);
	this->scaleVector = scale;
	this->UpdateMatrix();
}
void Transform::SetScale(const XMFLOAT3& scale)
{
	this->scale = scale;
	this->scaleVector = XMLoadFloat3(&this->scale);
	this->UpdateMatrix();
}
void Transform::SetScale(float xScale, float yScale, float zScale)
{
	this->scale.x = xScale;
	this->scale.y = yScale;
	this->scale.z = zScale;
	this->UpdateMatrix();
}
void Transform::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	if (lookAtPos.x == pos.x && lookAtPos.y == pos.y && lookAtPos.z == pos.z)
		return;

	lookAtPos.x = pos.x - lookAtPos.x;
	lookAtPos.y = pos.y - lookAtPos.y;
	lookAtPos.z = pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	SetRotation(pitch, yaw, 0.0f);
}

const XMVECTOR& Transform::GetForwardVector(bool omitY)
{
	if (omitY)
		return vec_forward_noY;
	else
		return vec_forward;
}

const XMVECTOR& Transform::GetRightVector(bool omitY)
{
	if (omitY)
		return vec_right_noY;
	else
		return vec_right;
}

const XMVECTOR& Transform::GetBackwardVector(bool omitY)
{
	if (omitY)
		return vec_backward_noY;
	else
		return vec_backward;
}

const XMVECTOR& Transform::GetLeftVector(bool omitY)
{
	if (omitY)
		return vec_left_noY;
	else
		return vec_left;
}
void Transform::UpdateDirectionVectors()
{
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, 0.0f);
	vec_forward = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	vec_backward = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	vec_left = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	vec_right = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrix);

	XMMATRIX vecRotationMatrix_noY = XMMatrixRotationRollPitchYaw(0.0f, rot.y, 0.0f);
	vec_forward_noY = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrix_noY);
	vec_backward_noY = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix_noY);
	vec_left_noY = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrix_noY);
	vec_right_noY = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrix_noY);
}
void Transform::UpdateMatrix()
{
	this->worldMatrix = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z) * XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->UpdateDirectionVectors();
}
