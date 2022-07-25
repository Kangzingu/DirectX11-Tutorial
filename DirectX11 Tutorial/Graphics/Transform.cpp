#include "Transform.h"
#include "../Utils/SimpleMath.h"

void Transform::Initialize(Vector3 position, Vector3 rotation, Vector3 scale)
{
	this->SetPosition(position);
	this->SetScale(scale);
}
void Transform::SetPosition(Vector3 position)
{
	this->position = position;
	this->translationMatrix = Matrix4x4::Translation(this->position);
	this->worldMatrix = this->translationMatrix * this->rotationMatrix * this->scalingMatrix;
}
void Transform::SetRotation(Vector3 rotation)
{
	this->rotation = rotation;
	this->rotationQuaternion = Vector4::Quaternion(Vector3::Normalize(rotation), Vector3::Magnitude(rotation));
	this->rotationQuaternion = Vector4::Normalize(this->rotationQuaternion);
	this->rotationMatrix = Matrix4x4::RotationQuaternion(this->rotationQuaternion);
	this->worldMatrix = this->translationMatrix * this->rotationMatrix * this->scalingMatrix;
	this->right = Vector3::Normalize(this->rotationMatrix * Vector3::Right());
	this->up = Vector3::Normalize(this->rotationMatrix * Vector3::Up());
	this->forward = Vector3::Normalize(this->rotationMatrix * Vector3::Forward());
}
void Transform::SetRotationQuaternion(Vector4 rotationQuaternion)
{
	this->rotationQuaternion = rotationQuaternion;
	this->rotationQuaternion = Vector4::Normalize(this->rotationQuaternion);
	this->rotationMatrix = Matrix4x4::RotationQuaternion(this->rotationQuaternion);
	this->worldMatrix = this->translationMatrix * this->rotationMatrix * this->scalingMatrix;
	this->right = Vector3::Normalize(this->rotationMatrix * Vector3::Right());
	this->up = Vector3::Normalize(this->rotationMatrix * Vector3::Up());
	this->forward = Vector3::Normalize(this->rotationMatrix * Vector3::Forward());
}
void Transform::SetScale(Vector3 scale)
{
	this->scale = scale;
	this->scalingMatrix = Matrix4x4::Scaling(this->scale);
	this->worldMatrix = this->translationMatrix * this->rotationMatrix * this->scalingMatrix;
}
void Transform::Translate(Vector3 position)
{
	this->position += position;
	this->translationMatrix = Matrix4x4::Translation(this->position);
	this->worldMatrix = this->translationMatrix * this->rotationMatrix * this->scalingMatrix;
}
void Transform::Rotate(Vector3 rotation)
{
	//this->rotationQuaternion = Vector4::CombineQuaternion(Vector4::Quaternion(Vector3::Normalize(rotation), Vector3::Magnitude(rotation)), this->rotationQuaternion);
	Vector4 addQuaternion= Vector4(rotation, 0);
	addQuaternion = (Vector4::CombineQuaternion(addQuaternion, this->rotationQuaternion)) / 2.0f;
	this->rotationQuaternion += addQuaternion;
	this->rotationQuaternion = Vector4::Normalize(this->rotationQuaternion);
	this->rotationMatrix = Matrix4x4::RotationQuaternion(this->rotationQuaternion);
	this->worldMatrix = this->translationMatrix * this->rotationMatrix * this->scalingMatrix;
	this->right = Vector3::Normalize(this->rotationMatrix * Vector3::Right());
	this->up = Vector3::Normalize(this->rotationMatrix * Vector3::Up());
	this->forward = Vector3::Normalize(this->rotationMatrix * Vector3::Forward());
}
void Transform::Scale(Vector3 scale)
{
	this->scale += scale;
	this->scalingMatrix = Matrix4x4::Scaling(this->scale);
	this->worldMatrix = this->translationMatrix * this->rotationMatrix * this->scalingMatrix;
}
Matrix4x4 Transform::GetWorldMatrix() { return worldMatrix; }
Matrix4x4 Transform::GetTranslationMatrix() { return translationMatrix; }
Matrix4x4 Transform::GetRotationMatrix() { return rotationMatrix; }
Matrix4x4 Transform::GetScalingMatrix() { return scalingMatrix; }
Vector3 Transform::GetPosition() { return position; }
Vector3 Transform::GetRotation()
{
	float t0 = 2.0f * (rotationQuaternion.w * rotationQuaternion.x + rotationQuaternion.y * rotationQuaternion.z);
	float t1 = 1.0f - 2.0f * (rotationQuaternion.x * rotationQuaternion.x + rotationQuaternion.y * rotationQuaternion.y);
	rotation.x = atan2(t0, t1);

	float t2 = 2.0f * (rotationQuaternion.w * rotationQuaternion.y - rotationQuaternion.z * rotationQuaternion.x);
	if (t2 > +1.0f)
		t2 = +1.0f;
	if (t2 < -1.0f)
		t2 = -1.0f;
	rotation.y = asin(t2);

	float  t3 = 2.0f * (rotationQuaternion.w * rotationQuaternion.z + rotationQuaternion.x * rotationQuaternion.y);
	float t4 = +1.0f - 2.0f * (rotationQuaternion.y * rotationQuaternion.y + rotationQuaternion.z * rotationQuaternion.z);
	rotation.z = atan2(t3, t4);
	return rotation;
}
Vector4 Transform::GetRotationQuaternion() { return rotationQuaternion; }
Vector3 Transform::GetScale() { return scale; }
Vector3 Transform::GetRight() { return right; }
Vector3 Transform::GetUp() { return up; }
Vector3 Transform::GetForward() { return forward; }