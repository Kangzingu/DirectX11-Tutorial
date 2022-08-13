#include "Rigidbody.h"
#include "../Object.h"

void Rigidbody::Initialize(float inverseMass, float damping, float angularDamping, Vector3 velocity, Vector3 angularVelocity, Matrix4x4 inertiaTensorInverse)
{
	m_inverseMass = inverseMass;
	m_damping = damping;
	m_angularDamping = angularDamping;
	m_velocity = velocity;
	m_angularVelocity = angularVelocity;
	m_inertiaTensorInverse = inertiaTensorInverse;
	ClearAccumulatedForce();
}
void Rigidbody::AddForce(Vector3 force)
{
	m_accumulatedForce += force;
}
void Rigidbody::AddForceAt(Vector3 force, Vector3 worldPoint)
{
	m_accumulatedForce += force;
	m_accumulatedTorque +=  (Vector3::Cross(worldPoint - m_object->m_transform.GetPosition(), force));
}
void Rigidbody::AddTorqueAt(Vector3 force, Vector3 worldPoint)
{
	m_accumulatedTorque += (Vector3::Cross(worldPoint - m_object->m_transform.GetPosition(), force));
}
void Rigidbody::Update(float deltaTime)
{
	if (m_isKinematic == false)
	{
		m_lastFrameAcceleration = m_accumulatedForce * m_inverseMass;

		m_velocity = (m_velocity + (m_lastFrameAcceleration) * deltaTime) * pow(m_damping, deltaTime);
		m_object->m_transform.Translate(m_velocity * deltaTime);

		m_angularVelocity = (m_angularVelocity + (GetWorldInertiaTensorInverse() * m_accumulatedTorque) * deltaTime) * pow(m_angularDamping, deltaTime);
		m_object->m_transform.Rotate(m_angularVelocity * deltaTime);
	}
	ClearAccumulatedForce();
}
void Rigidbody::SetKinematic(bool isKinematic)
{
	m_isKinematic = isKinematic;
}
void Rigidbody::SetEnabled(bool isEnabled)
{
	m_isEnabled = isEnabled;
}
void Rigidbody::SetInverseMass(float inverseMass)
{
	m_inverseMass = inverseMass;
}
void Rigidbody::SetDamping(float damping)
{
	m_damping = damping;
}
void Rigidbody::SetAngularDamping(float angularDamping)
{
	m_angularDamping = angularDamping;
}
void Rigidbody::SetVelocity(Vector3 velocity)
{
	m_velocity = velocity;
}
void Rigidbody::AddVelocity(Vector3 velocity)
{
	m_velocity += velocity;
}
void Rigidbody::SetAngularVelocity(Vector3 angularVelocity)
{
	m_angularVelocity = angularVelocity;
}
void Rigidbody::AddAngularVelocity(Vector3 angularVelocity)
{
	m_angularVelocity += angularVelocity;
}
void Rigidbody::SetAccumulatedForce(Vector3 accumulatedForce)
{
	m_accumulatedForce = accumulatedForce;
}
void Rigidbody::SetAccumulatedTorque(Vector3 accumulatedTorque)
{
	m_accumulatedTorque = accumulatedTorque;
}
void Rigidbody::SetInertiaTensorInverse(Matrix4x4 inertiaTensorInverse)
{
	m_inertiaTensorInverse = inertiaTensorInverse;
}
void Rigidbody::ClearAccumulatedForce()
{
	m_accumulatedForce = Vector3::Zero();
	m_accumulatedTorque = Vector3::Zero();
}
bool Rigidbody::IsKinematic() { return m_isKinematic; }
bool Rigidbody::IsEnabled() { return m_isEnabled; }
float Rigidbody::GetInverseMass(){return m_inverseMass;}
float Rigidbody::GetDamping(){return m_damping;}
float Rigidbody::GetangularDamping(){return m_angularDamping;}
Vector3 Rigidbody::GetVelocity(){return m_velocity;}
Vector3 Rigidbody::GetAngularVelocity(){return m_angularVelocity;}
Vector3 Rigidbody::GetAccumulatedForce(){return m_accumulatedForce;}
Vector3 Rigidbody::GetAccumulatedTorque(){return m_accumulatedTorque;}
Vector3 Rigidbody::GetLastFrameAcceleration(){return m_lastFrameAcceleration;}
Matrix4x4 Rigidbody::GetInertiaTensorInverse(){return m_inertiaTensorInverse;}
Matrix4x4 Rigidbody::GetWorldInertiaTensorInverse()
{
	/*Matrix4x4 iitBody = m_inertiaTensorInverse;
	Matrix4x4 iitWorld = Matrix4x4::Identity();
	Matrix4x4 rotmat = m_object->m_transform.GetRotationMatrix();
	float t4 = rotmat.m00 * iitBody.m00 +
		rotmat.m01 * iitBody.m10 +
		rotmat.m02 * iitBody.m20;
	float t9 = rotmat.m00 * iitBody.m01 +
		rotmat.m01 * iitBody.m11 +
		rotmat.m02 * iitBody.m21;
	float t14 = rotmat.m00 * iitBody.m02 +
		rotmat.m01 * iitBody.m12 +
		rotmat.m02 * iitBody.m22;
	float t28 = rotmat.m10 * iitBody.m00 +
		rotmat.m11 * iitBody.m10 +
		rotmat.m12 * iitBody.m20;
	float t33 = rotmat.m10 * iitBody.m01 +
		rotmat.m11 * iitBody.m11 +
		rotmat.m12 * iitBody.m21;
	float t38 = rotmat.m10 * iitBody.m02 +
		rotmat.m11 * iitBody.m12 +
		rotmat.m12 * iitBody.m22;
	float t52 = rotmat.m20 * iitBody.m00 +
		rotmat.m21 * iitBody.m10 +
		rotmat.m22 * iitBody.m20;
	float t57 = rotmat.m20 * iitBody.m01 +
		rotmat.m21 * iitBody.m11 +
		rotmat.m22 * iitBody.m21;
	float t62 = rotmat.m20 * iitBody.m02 +
		rotmat.m21 * iitBody.m12 +
		rotmat.m22 * iitBody.m22;
	iitWorld.m00 = t4 * rotmat.m00 +
		t9 * rotmat.m01 +
		t14 * rotmat.m02;
	iitWorld.m01 = t4 * rotmat.m10 +
		t9 * rotmat.m11 +
		t14 * rotmat.m12;
	iitWorld.m02 = t4 * rotmat.m20 +
		t9 * rotmat.m21 +
		t14 * rotmat.m22;
	iitWorld.m10 = t28 * rotmat.m00 +
		t33 * rotmat.m01 +
		t38 * rotmat.m02;
	iitWorld.m11 = t28 * rotmat.m10 +
		t33 * rotmat.m11 +
		t38 * rotmat.m12;
	iitWorld.m12 = t28 * rotmat.m20 +
		t33 * rotmat.m21 +
		t38 * rotmat.m22;
	iitWorld.m20 = t52 * rotmat.m00 +
		t57 * rotmat.m01 +
		t62 * rotmat.m02;
	iitWorld.m21 = t52 * rotmat.m10 +
		t57 * rotmat.m11 +
		t62 * rotmat.m12;
	iitWorld.m22 = t52 * rotmat.m20 +
		t57 * rotmat.m21 +
		t62 * rotmat.m22;
	m_worldInertiaTensorInverse = iitWorld;
	return m_worldInertiaTensorInverse;*/
	m_worldInertiaTensorInverse = m_object->m_transform.GetRotationMatrix() * m_inertiaTensorInverse * m_object->m_transform.GetRotationMatrix().Transpose();
	return  m_worldInertiaTensorInverse;
}

// 힘 = 질량 * 가속도
// 임펄스 힘 = 질량 * 속도
// 토크 = 관성텐서 * 각가속도
// 임펄스 토크 = 관성텐서 * 각속도
// 각속도 = 관성텐서^-1 * (상대위치 X 임펄스 힘) 인듯..?