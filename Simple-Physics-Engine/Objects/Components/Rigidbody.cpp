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

	m_isAwake = true;
}
void Rigidbody::AddForceAt(Vector3 force, Vector3 worldPoint)
{
	m_accumulatedForce += force;
	m_accumulatedTorque += (Vector3::Cross(worldPoint - m_object->m_transform.GetPosition(), force));

	m_isAwake = true;
}
void Rigidbody::AddTorqueAt(Vector3 force, Vector3 worldPoint)
{
	m_accumulatedTorque += (Vector3::Cross(worldPoint - m_object->m_transform.GetPosition(), force));

	m_isAwake = true;
}
void Rigidbody::Update(float deltaTime)
{
	if (m_isKinematic == true)
		return;

	if (m_isAwake == false)
		return;

	m_lastFrameAcceleration = m_acceleration;
	m_lastFrameAcceleration += m_accumulatedForce * m_inverseMass;

	m_velocity = (m_velocity + (m_lastFrameAcceleration)*deltaTime) * pow(m_damping, deltaTime);
	m_angularVelocity = (m_angularVelocity + (GetWorldInertiaTensorInverse() * m_accumulatedTorque) * deltaTime) * pow(m_angularDamping, deltaTime);

	m_object->m_transform.Translate(m_velocity * deltaTime);
	m_object->m_transform.Rotate(m_angularVelocity * deltaTime);

	ClearAccumulatedForce();

	float currentMotion = Vector3::SquareMagnitude(m_velocity) + Vector3::SquareMagnitude(m_angularVelocity);

	float bias = pow(0.5f, deltaTime);
	m_motion = bias * m_motion + (1 - bias) * currentMotion;

	if (m_motion < m_sleepEpsilon)
		SetAwake(false);
	else if (m_motion > 10 * m_sleepEpsilon)
		m_motion = 10 * m_sleepEpsilon;
}
void Rigidbody::SetKinematic(bool isKinematic)
{
	m_isKinematic = isKinematic;
}
void Rigidbody::SetAwake(bool isAwake)
{
	if (isAwake)
	{
		m_isAwake = true;
		m_motion = m_sleepEpsilon * 2.0f;
	}
	else
	{
		m_isAwake = false;
		m_velocity = Vector3::Zero();
		m_angularVelocity = Vector3::Zero();
	}
}
void Rigidbody::SetMotion(float motion)
{
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
void Rigidbody::SetAcceleration(Vector3 acceleration)
{
	m_acceleration = acceleration;
}
void Rigidbody::SetVelocity(Vector3 velocity)
{
	m_velocity = velocity;
}
void Rigidbody::AddVelocity(Vector3 velocity)
{
	m_velocity += velocity;

	m_isAwake = true;
}
void Rigidbody::SetAngularVelocity(Vector3 angularVelocity)
{
	m_angularVelocity = angularVelocity;
}
void Rigidbody::AddAngularVelocity(Vector3 angularVelocity)
{
	m_angularVelocity += angularVelocity;

	m_isAwake = true;
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
bool Rigidbody::IsKinematic()
{
	return m_isKinematic;
}
bool Rigidbody::IsAwake()
{
	return m_isAwake;
}
float Rigidbody::GetMotion()
{
	return m_motion;
}
float Rigidbody::GetInverseMass()
{
	return m_inverseMass;
}
float Rigidbody::GetDamping()
{
	return m_damping;
}
float Rigidbody::GetAngularDamping()
{
	return m_angularDamping;
}
Vector3& Rigidbody::GetAcceleration()
{
	return m_acceleration;
}
Vector3& Rigidbody::GetVelocity()
{
	return m_velocity;
}
Vector3& Rigidbody::GetAngularVelocity()
{
	return m_angularVelocity;
}
Vector3& Rigidbody::GetAccumulatedForce()
{
	return m_accumulatedForce;
}
Vector3& Rigidbody::GetAccumulatedTorque()
{
	return m_accumulatedTorque;
}
Vector3& Rigidbody::GetLastFrameAcceleration()
{
	return m_lastFrameAcceleration;
}
Matrix4x4& Rigidbody::GetInertiaTensorInverse()
{
	return m_inertiaTensorInverse;
}
Matrix4x4& Rigidbody::GetWorldInertiaTensorInverse()
{
	m_worldInertiaTensorInverse = m_object->m_transform.GetRotationMatrix() * m_inertiaTensorInverse * m_object->m_transform.GetRotationMatrix().Transpose();
	return  m_worldInertiaTensorInverse;
}

// 힘 = 질량 * 가속도
// 임펄스 힘 = 질량 * 속도
// 토크 = 관성텐서 * 각가속도
// 임펄스 토크 = 관성텐서 * 각속도
// 각속도 = 관성텐서^-1 * (상대위치 X 임펄스 힘) 인듯..?