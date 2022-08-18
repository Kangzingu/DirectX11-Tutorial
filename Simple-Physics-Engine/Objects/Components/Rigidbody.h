#pragma once
#include <DirectXMath.h>
#include "Transform.h"
#include "Components.h"
#include "../Utils/SimpleMath.h"

class Rigidbody : public Components
{
private:
	bool m_isKinematic = false;
	bool m_isAwake = true;

	float m_sleepEpsilon = 0.5f;
	float m_motion = 0.6f;
	float m_inverseMass;
	float m_damping = 0.95f;
	float m_angularDamping = 0.95f;
	
	Vector3 m_acceleration = Vector3::Zero();
	Vector3 m_velocity = Vector3::Zero();// m/s ¥‹¿ß
	Vector3 m_angularVelocity = Vector3::Zero();
	
	Vector3 m_accumulatedForce = Vector3::Zero();
	Vector3 m_accumulatedTorque = Vector3::Zero();
	
	Vector3 m_lastFrameAcceleration = Vector3::Zero();

	Matrix4x4 m_inertiaTensorInverse = Matrix4x4::Identity();
	Matrix4x4 m_worldInertiaTensorInverse = Matrix4x4::Identity();

public:
	bool IsKinematic();
	bool IsAwake();

	float GetMotion();
	float GetInverseMass();
	float GetDamping();
	float GetAngularDamping();

	Vector3& GetAcceleration();
	Vector3& GetVelocity();
	Vector3& GetAngularVelocity();
	Vector3& GetAccumulatedForce();
	Vector3& GetAccumulatedTorque();
	Vector3& GetLastFrameAcceleration();

	Matrix4x4& GetInertiaTensorInverse();
	Matrix4x4& GetWorldInertiaTensorInverse();

public:
	void Initialize(float inverseMass, float damping, float angularDamping, Vector3 velocity, Vector3 rotationVelocity, Matrix4x4 inertiaTensorInverse);
	void Update(float deltaTime);

	void SetKinematic(bool isKinematic);
	void SetAwake(bool isAwake);

	void SetMotion(float motion);
	void SetInverseMass(float inverseMass);
	void SetDamping(float damping);
	void SetAngularDamping(float angularDamping);

	void SetAcceleration(Vector3 acceleration);
	void SetVelocity(Vector3 velocity);
	void AddVelocity(Vector3 velocity);
	void SetAngularVelocity(Vector3 angularVelocity);
	void AddAngularVelocity(Vector3 angularVelocity);
	void SetAccumulatedForce(Vector3 accumulatedForce);
	void SetAccumulatedTorque(Vector3 accumulatedTorque);

	void SetInertiaTensorInverse(Matrix4x4 inertiaTensor);

	void AddForce(Vector3 force);
	void AddForceAt(Vector3 force, Vector3 worldPoint);
	void AddTorqueAt(Vector3 force, Vector3 worldPoint);
	void ClearAccumulatedForce();
};