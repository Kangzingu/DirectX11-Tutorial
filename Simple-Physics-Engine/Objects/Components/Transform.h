#pragma once
#include "Components.h"
#include "Model/Model.h"

class Transform : public Components
{
private:
	Vector3 m_position = Vector3::Zero();
	Quaternion m_orientation = Quaternion::Identity();
	Vector3 m_scale = Vector3::One();

	Vector3 m_right = Vector3::Right();
	Vector3 m_up = Vector3::Up();
	Vector3 m_forward = Vector3::Forward();

	Matrix4x4 m_worldMatrix = Matrix4x4::Identity();
	Matrix4x4 m_translationMatrix = Matrix4x4::Identity();
	Matrix4x4 m_rotationMatrix = Matrix4x4::Identity();
	Matrix4x4 m_scalingMatrix = Matrix4x4::Identity();

public:
	Matrix4x4& GetWorldMatrix();
	Matrix4x4& GetTranslationMatrix();
	Matrix4x4& GetRotationMatrix();
	Matrix4x4& GetScalingMatrix();

	Vector3& GetPosition();
	Vector3& GetRotation();
	Quaternion& GetOrientation();
	Vector3& GetScale();

	Vector3& GetRight();
	Vector3& GetUp();
	Vector3& GetForward();

public:
	void Initialize(Vector3 position, Vector3 rotation, Vector3 scale);

	void SetPosition(Vector3 position);
	void SetRotation(Quaternion orientation);
	void SetScale(Vector3 scale);

	void Translate(Vector3 position);
	void Rotate(Vector3 rotation);
	void Rotate(Quaternion orientation);
	void Scale(Vector3 scale);
};