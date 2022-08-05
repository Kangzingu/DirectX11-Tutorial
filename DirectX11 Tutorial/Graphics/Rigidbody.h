#pragma once
#include <DirectXMath.h>
#include "../Utils/SimpleMath.h"
#include "Transform.h"

class Object;
class Rigidbody
{
private:
	bool isEnabled = true;
	
	float inverseMass;
	float damping = 0.95f;
	float angularDamping = 0.95f;
	Vector3 velocity = Vector3::Zero();// m/s ¥‹¿ß
	Vector3 angularVelocity = Vector3::Zero();
	
	Vector3 accumulatedForce = Vector3::Zero();
	Vector3 accumulatedTorque = Vector3::Zero();
	
	Matrix4x4 inertiaTensor = Matrix4x4::Identity();

public:
	Object* object;

	bool IsEnabled();

	float GetInverseMass();
	float GetDamping();
	float GetangularDamping();

	Vector3 GetVelocity();
	Vector3 GetAngularVelocity();
	Vector3 GetAccumulatedForce();
	Vector3 GetAccumulatedTorque();

	Matrix4x4 GetInertiaTensor();

public:
	void Initialize(float mass, float damping, float angularDamping, Vector3 velocity, Vector3 rotationVelocity, Matrix4x4 inertiaTensor);
	void Update(float deltaTime);
	
	void SetEnabled(bool isEnabled);

	void SetInverseMass(float mass);
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