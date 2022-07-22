#pragma once
#include <DirectXMath.h>
#include "../Utils/SimpleMath.h"
#include "Transform.h"
using namespace DirectX;

class Rigidbody
{
public:
	void Initialize(Vector3 velocity, Vector3 accumulatedForce, float damping, float mass, bool isKinematic);
	void Update(Transform& transform, float deltaTime);
	void AddForce(Vector3 force);
	void AddForceAt(Vector3 force, Vector3 worldPoint, Transform transform);
	void AddTorque(Vector3 force, Vector3 worldPoint, Transform transform);
	void ClearAccumulatedForce();
	Vector3 orientation = XMQuaternionIdentity();
	Vector3 velocity;// m/s 단위
	Vector3 rotationVelocity;
	Vector3 accumulatedForce;
	Vector3 accumulatedTorque;
	float damping;
	float angularDamping;
	float mass;// kg단위
	Matrix4x4 momentOfInertia;
	bool isKinematic;
	
};