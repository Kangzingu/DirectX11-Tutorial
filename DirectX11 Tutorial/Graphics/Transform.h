#pragma once
#include "Model.h"

class Transform
{
public:
	//const Vector3 GetPositionVector() const;
	//const XMFLOAT3& GetPositionFloat3() const;
	//const XMVECTOR& GetRotationVector() const;
	//const XMFLOAT3& GetRotationFloat3() const;
	//const XMVECTOR& GetScaleVector() const;
	//const XMFLOAT3& GetScaleFloat3() const;

	//void SetPosition(const XMVECTOR& pos);
	//void SetPosition(const XMFLOAT3& pos);
	//void SetPosition(float x, float y, float z);
	//void SetRotation(const XMVECTOR& rot);
	//void SetRotation(const XMFLOAT3& rot);
	//void SetRotation(float x, float y, float z);
	//void SetScale(const XMVECTOR& scale);
	//void SetScale(const XMFLOAT3& scale);
	//void SetScale(float x, float y, float z = 1.0f);

	//void AdjustPosition(const XMVECTOR& pos);
	//void AdjustPosition(const XMFLOAT3& pos);
	//void AdjustPosition(float x, float y, float z);
	//void AdjustRotation(const XMVECTOR& rot);
	//void AdjustRotation(const XMFLOAT3& rot);
	//void AdjustRotation(float x, float y, float z);

	//const XMVECTOR& GetForwardVector(bool omitY = false);
	//const XMVECTOR& GetRightVector(bool omitY = false);
	//const XMVECTOR& GetBackwardVector(bool omitY = false);
	//const XMVECTOR& GetLeftVector(bool omitY = false);
	//void UpdateDirectionVectors();
	//const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	//const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	//const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	//const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	Matrix4x4 worldMatrix;
	void UpdateMatrix();

	//XMVECTOR vec_forward;
	//XMVECTOR vec_left;
	//XMVECTOR vec_right;
	//XMVECTOR vec_backward;

	//XMVECTOR vec_forward_noY;
	//XMVECTOR vec_left_noY;
	//XMVECTOR vec_right_noY;
	//XMVECTOR vec_backward_noY;
	
	void SetLookAtPos(Vector3 lookAtPos);
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;
};