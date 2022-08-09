#pragma once
#include <DirectXMath.h>
#include "../Utils/SimpleMath.h"
#include "Transform.h"
#include "Components.h"

class Rigidbody : public Components
{
private:
	bool m_isEnabled = true;
	bool m_isKinematic = false;
	
	float m_inverseMass;
	float m_damping = 0.95f;
	float m_angularDamping = 0.95f;
	Vector3 m_velocity = Vector3::Zero();// m/s ¥‹¿ß
	Vector3 m_angularVelocity = Vector3::Zero();
	
	Vector3 m_accumulatedForce = Vector3::Zero();
	Vector3 m_accumulatedTorque = Vector3::Zero();
	
	Matrix4x4 m_inertiaTensor = Matrix4x4::Identity();

public:
	bool IsEnabled();
	bool IsKinematic();

	float GetInverseMass();
	float GetDamping();
	float GetangularDamping();

	Vector3 GetVelocity();
	Vector3 GetAngularVelocity();
	Vector3 GetAccumulatedForce();
	Vector3 GetAccumulatedTorque();

	Matrix4x4 GetInertiaTensor();
	Matrix4x4 GetWorldInertiaTensorInverse();

public:
	void Initialize(float inverseMass, float damping, float angularDamping, Vector3 velocity, Vector3 rotationVelocity, Matrix4x4 inertiaTensor);
	void Update(float deltaTime);

	void SetKinematic(bool isKinematic);
	void SetEnabled(bool isEnabled);

	void SetInverseMass(float inverseMass);
	void SetDamping(float damping);
	void SetAngularDamping(float angularDamping);

	void SetVelocity(Vector3 velocity);
	void AddVelocity(Vector3 velocity);
	void SetAngularVelocity(Vector3 angularVelocity);
	void AddAngularVelocity(Vector3 angularVelocity);
	void SetAccumulatedForce(Vector3 accumulatedForce);
	void SetAccumulatedTorque(Vector3 accumulatedTorque);

	void SetInertiaTensor(Matrix4x4 inertiaTensor);

	void AddForce(Vector3 force);
	void AddForceAt(Vector3 force, Vector3 worldPoint);
	void AddTorqueAt(Vector3 force, Vector3 worldPoint);
	void ClearAccumulatedForce();
};