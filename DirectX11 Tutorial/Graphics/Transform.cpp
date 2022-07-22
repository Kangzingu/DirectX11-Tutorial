#include "Transform.h"
#include "../Utils/SimpleMath.h"

void Transform::UpdateMatrix()
{
	right = Matrix4x4::Rotation(rotation) * Vector3::Right();
	up = Matrix4x4::Rotation(rotation) * Vector3::Up();
	forward = Matrix4x4::Rotation(rotation) * Vector3::Forward();
	//worldMatrix = Matrix4x4::Translation(position) * Matrix4x4::Rotation(rotation) * Matrix4x4::Scaling(scale);
	worldMatrix = Matrix4x4::Translation(position) * Matrix4x4::Quaternion(Vector3::Normalize(rotation), Vector3::Magnitude(rotation)) * Matrix4x4::Scaling(scale);
}