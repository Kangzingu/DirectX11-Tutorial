#include "Transform.h"
#include "../Utils/SimpleMath.h"

//const XMVECTOR& Transform::GetPositionVector() const
//{
//	return this->posVector;
//}
//const XMFLOAT3& Transform::GetPositionFloat3() const
//{
//	return this->pos;
//}
//const XMVECTOR& Transform::GetRotationVector() const
//{
//	return this->rotVector;
//}
//const XMFLOAT3& Transform::GetRotationFloat3() const
//{
//	return this->rot;
//}
//const XMVECTOR& Transform::GetScaleVector() const
//{
//	return this->scaleVector;
//}
//const XMFLOAT3& Transform::GetScaleFloat3() const
//{
//	return this->scale;
//}
//void Transform::SetPosition(const XMVECTOR& pos)
//{
//	XMStoreFloat3(&this->pos, pos);
//	this->posVector = pos;
//	this->UpdateMatrix();
//}
//void Transform::SetPosition(const XMFLOAT3& pos)
//{
//	this->pos = pos;
//	this->posVector = XMLoadFloat3(&this->pos);
//	this->UpdateMatrix();
//}
//void Transform::SetPosition(float x, float y, float z)
//{
//	this->pos = XMFLOAT3(x, y, z);
//	this->posVector = XMLoadFloat3(&this->pos);
//	this->UpdateMatrix();
//}
//void Transform::AdjustPosition(const XMVECTOR& pos)
//{
//	this->posVector += pos;
//	XMStoreFloat3(&this->pos, this->posVector);
//	this->UpdateMatrix();
//}
//void Transform::AdjustPosition(const XMFLOAT3& pos)
//{
//	this->pos.x += pos.x;
//	this->pos.y += pos.y;
//	this->pos.z += pos.z;
//	this->posVector = XMLoadFloat3(&this->pos);
//	this->UpdateMatrix();
//}
//void Transform::AdjustPosition(float x, float y, float z)
//{
//	this->pos.x += x;
//	this->pos.y += y;
//	this->pos.z += z;
//	this->posVector = XMLoadFloat3(&this->pos);
//	this->UpdateMatrix();
//}
//void Transform::SetRotation(const XMVECTOR& rot)
//{
//	this->rotVector = rot;
//	XMStoreFloat3(&this->rot, rot);
//	this->UpdateMatrix();
//}
//void Transform::SetRotation(const XMFLOAT3& rot)
//{
//	this->rot = rot;
//	this->rotVector = XMLoadFloat3(&this->rot);
//	this->UpdateMatrix();
//}
//void Transform::SetRotation(float x, float y, float z)
//{
//	this->rot = XMFLOAT3(x, y, z);
//	this->rotVector = XMLoadFloat3(&this->rot);
//	this->UpdateMatrix();
//}
//void Transform::AdjustRotation(const XMVECTOR& rot)
//{
//	this->rotVector += rot;
//	XMStoreFloat3(&this->rot, this->rotVector);
//	this->UpdateMatrix();
//}
//void Transform::AdjustRotation(const XMFLOAT3& rot)
//{
//	this->rot.x += rot.x;
//	this->rot.y += rot.y;
//	this->rot.z += rot.z;
//	this->rotVector = XMLoadFloat3(&this->rot);
//	this->UpdateMatrix();
//}
//void Transform::AdjustRotation(float x, float y, float z)
//{
//	this->rot.x += x;
//	this->rot.y += y;
//	this->rot.z += z;
//	this->rotVector = XMLoadFloat3(&this->rot);
//	this->UpdateMatrix();
//}
//void Transform::SetScale(const XMVECTOR& scale)
//{
//	XMStoreFloat3(&this->scale, scale);
//	this->scaleVector = scale;
//	this->UpdateMatrix();
//}
//void Transform::SetScale(const XMFLOAT3& scale)
//{
//	this->scale = scale;
//	this->scaleVector = XMLoadFloat3(&this->scale);
//	this->UpdateMatrix();
//}
//void Transform::SetScale(float xScale, float yScale, float zScale)
//{
//	this->scale.x = xScale;
//	this->scale.y = yScale;
//	this->scale.z = zScale;
//	this->UpdateMatrix();
//}
void Transform::SetLookAtPos(Vector3 target)
{
	if (target.x == position.x && target.y == position.y && target.z == position.z)
		return;

	target.x = position.x - target.x;
	target.y = position.y - target.y;
	target.z = position.z - target.z;

	float rotX = 0.0f;
	if (target.y != 0.0f)
	{
		const float distance = sqrt(target.x * target.x + target.z * target.z);
		rotX = atan(target.y / distance);
	}

	float rotY = 0.0f;
	if (target.x != 0.0f)
	{
		rotY = atan(target.x / target.z);
	}
	if (target.z > 0)
		rotY += XM_PI;

	rotation = { rotX, rotY, 0.0f };
}

//XMMATRIX vecRotationMatrix_noY = XMMatrixRotationRollPitchYaw(0.0f, rot.y, 0.0f);
//vec_forward_noY = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrix_noY);
//vec_backward_noY = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix_noY);
//vec_left_noY = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrix_noY);
//vec_right_noY = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrix_noY);
void Transform::UpdateMatrix()
{
	this->worldMatrix = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z) * XMMatrixRotationRollPitchYaw(this->rotation.x, this->rotation.y, this->rotation.z) * XMMatrixTranslation(this->position.x, this->position.y, this->position.z);
}
