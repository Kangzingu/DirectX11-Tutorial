#include "Transform.h"
#include "../Object.h"
#include "../Utils/SimpleMath.h"

void Transform::Initialize(Vector3 position, Vector3 rotation, Vector3 scale)
{
	SetPosition(position);
	SetScale(scale);
}
void Transform::SetPosition(Vector3 position)
{
	m_position = position;
	m_translationMatrix = Matrix4x4::Translation(m_position);
	m_worldMatrix = m_translationMatrix * m_rotationMatrix * m_scalingMatrix;
}
void Transform::SetRotation(Quaternion orientation)
{
	m_orientation = Quaternion::Normalize(orientation);
	m_rotationMatrix = Matrix4x4::Quaternion(m_orientation);
	m_worldMatrix = m_translationMatrix * m_rotationMatrix * m_scalingMatrix;
	m_right = Vector3::Normalize(m_rotationMatrix * Vector3::Right());
	m_up = Vector3::Normalize(m_rotationMatrix * Vector3::Up());
	m_forward = Vector3::Normalize(m_rotationMatrix * Vector3::Forward());
}
void Transform::SetScale(Vector3 scale)
{
	m_scale = scale;
	m_scalingMatrix = Matrix4x4::Scaling(m_scale);
	m_worldMatrix = m_translationMatrix * m_rotationMatrix * m_scalingMatrix;

	if (m_object != nullptr)
		m_object->m_collider.UpdateBound();
}
void Transform::Translate(Vector3 position)
{
	m_position += position;
	m_translationMatrix = Matrix4x4::Translation(m_position);
	m_worldMatrix = m_translationMatrix * m_rotationMatrix * m_scalingMatrix;
}
void Transform::Rotate(Vector3 rotation)
{
	m_orientation += rotation;
	m_orientation = Quaternion::Normalize(m_orientation);
	m_rotationMatrix = Matrix4x4::Quaternion(m_orientation);
	m_worldMatrix = m_translationMatrix * m_rotationMatrix * m_scalingMatrix;
	m_right = Vector3::Normalize(m_rotationMatrix * Vector3::Right());
	m_up = Vector3::Normalize(m_rotationMatrix * Vector3::Up());
	m_forward = Vector3::Normalize(m_rotationMatrix * Vector3::Forward());
}
void Transform::Rotate(Quaternion rotation)
{
	m_orientation *= rotation;
	m_orientation = Quaternion::Normalize(m_orientation);
	m_rotationMatrix = Matrix4x4::Quaternion(m_orientation);
	m_worldMatrix = m_translationMatrix * m_rotationMatrix * m_scalingMatrix;
	m_right = Vector3::Normalize(m_rotationMatrix * Vector3::Right());
	m_up = Vector3::Normalize(m_rotationMatrix * Vector3::Up());
	m_forward = Vector3::Normalize(m_rotationMatrix * Vector3::Forward());
}
void Transform::Scale(Vector3 scale)
{
	m_scale += scale;
	m_scalingMatrix = Matrix4x4::Scaling(m_scale);
	m_worldMatrix = m_translationMatrix * m_rotationMatrix * m_scalingMatrix;

	if (m_object != nullptr)
		m_object->m_collider.UpdateBound();
}
Matrix4x4& Transform::GetWorldMatrix()
{
	return m_worldMatrix;
}
Matrix4x4& Transform::GetTranslationMatrix()
{
	return m_translationMatrix;
}
Matrix4x4& Transform::GetRotationMatrix()
{
	return m_rotationMatrix;
}
Matrix4x4& Transform::GetScalingMatrix()
{
	return m_scalingMatrix;
}
Vector3& Transform::GetPosition()
{
	return m_position;
}
Vector3& Transform::GetRotation()
{
	Vector3 rotation;
	float t0 = 2.0f * (m_orientation.r * m_orientation.i + m_orientation.j * m_orientation.k);
	float t1 = 1.0f - 2.0f * (m_orientation.i * m_orientation.i + m_orientation.j * m_orientation.j);
	rotation.x = General::RadianToDegree(atan2(t0, t1));

	float t2 = 2.0f * (m_orientation.r * m_orientation.j - m_orientation.k * m_orientation.i);
	if (t2 > +1.0f)
		t2 = +1.0f;
	if (t2 < -1.0f)
		t2 = -1.0f;
	rotation.y = General::RadianToDegree(asin(t2));

	float t3 = 2.0f * (m_orientation.r * m_orientation.k + m_orientation.i * m_orientation.j);
	float t4 = +1.0f - 2.0f * (m_orientation.j * m_orientation.j + m_orientation.k * m_orientation.k);
	rotation.z = General::RadianToDegree(atan2(t3, t4));
	return rotation;
}
Quaternion& Transform::GetOrientation()
{
	return m_orientation;
}
Vector3& Transform::GetScale()
{
	return m_scale;
}
Vector3& Transform::GetRight()
{
	return m_right;
}
Vector3& Transform::GetUp()
{
	return m_up;
}
Vector3& Transform::GetForward()
{
	return m_forward;
}