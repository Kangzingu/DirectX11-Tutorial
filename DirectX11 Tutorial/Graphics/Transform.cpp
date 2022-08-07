#include "Object.h"
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
void Transform::SetRotation(Quaternion orientation)
{
	this->orientation = Quaternion::Normalize(orientation);
	this->rotationMatrix = Matrix4x4::Quaternion(this->orientation);
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
	this->orientation += rotation;
	this->orientation = Quaternion::Normalize(this->orientation);
	this->rotationMatrix = Matrix4x4::Quaternion(this->orientation);
	this->worldMatrix = this->translationMatrix * this->rotationMatrix * this->scalingMatrix;
	this->right = Vector3::Normalize(this->rotationMatrix * Vector3::Right());
	this->up = Vector3::Normalize(this->rotationMatrix * Vector3::Up());
	this->forward = Vector3::Normalize(this->rotationMatrix * Vector3::Forward());
}
void Transform::Rotate(Quaternion rotation)
{
	this->orientation *= rotation;
	this->orientation = Quaternion::Normalize(this->orientation);
	this->rotationMatrix = Matrix4x4::Quaternion(this->orientation);
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
Matrix4x4& Transform::GetWorldMatrix() { return worldMatrix; }
Matrix4x4& Transform::GetTranslationMatrix() { return translationMatrix; }
Matrix4x4& Transform::GetRotationMatrix() { return rotationMatrix; }
Matrix4x4& Transform::GetScalingMatrix() { return scalingMatrix; }
Vector3& Transform::GetPosition() { return position; }
Vector3& Transform::GetRotation()
{
	Vector3 rotation;
	float t0 = 2.0f * (orientation.r * orientation.i + orientation.j * orientation.k);
	float t1 = 1.0f - 2.0f * (orientation.i * orientation.i + orientation.j * orientation.j);
	rotation.x = General::RadianToDegree(atan2(t0, t1));

	float t2 = 2.0f * (orientation.r * orientation.j - orientation.k * orientation.i);
	if (t2 > +1.0f)
		t2 = +1.0f;
	if (t2 < -1.0f)
		t2 = -1.0f;
	rotation.y = General::RadianToDegree(asin(t2));

	float t3 = 2.0f * (orientation.r * orientation.k + orientation.i * orientation.j);
	float t4 = +1.0f - 2.0f * (orientation.j * orientation.j + orientation.k * orientation.k);
	rotation.z = General::RadianToDegree(atan2(t3, t4));
	return rotation;
}
Quaternion& Transform::GetOrientation() { return orientation; }
Vector3& Transform::GetScale() { return scale; }
Vector3& Transform::GetRight() { return right; }
Vector3& Transform::GetUp() { return up; }
Vector3& Transform::GetForward() { return forward; }