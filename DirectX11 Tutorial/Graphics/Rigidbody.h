#pragma once
#include <DirectXMath.h>
#include "../Utils/SimpleMath.h"
#include "Transform.h"
using namespace DirectX;

class Rigidbody
{
public:
	void Initialize(bool isKinematic, float mass, float damping, float angularDamping, Vector3 velocity, Vector3 rotationVelocity, Matrix4x4 momentOfInertia);
	void Update(Transform& transform, float deltaTime);
	void AddForce(Vector3 force);
	void AddForceAt(Vector3 force, Vector3 worldPoint, Transform transform);
	void AddTorqueAt(Vector3 force, Vector3 worldPoint, Transform transform);
	void ClearAccumulatedForce();

	bool isEnabled = true;
	bool isKinematic = false;
	float mass = 1.0f;// kg단위
	float damping = 0.95f;
	float angularDamping = 0.95f;
	Vector3 velocity = Vector3::Zero();// m/s 단위
	Vector3 angularVelocity = Vector3::Zero();
	Vector3 accumulatedForce = Vector3::Zero();
	Vector3 accumulatedTorque = Vector3::Zero();
	Matrix4x4 inertiaTensor = Matrix4x4::Identity();
};