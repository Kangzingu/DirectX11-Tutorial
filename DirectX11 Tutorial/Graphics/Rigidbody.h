#pragma once
#include <DirectXMath.h>
#include "../Utils/SimpleMath.h"
#include "Transform.h"
using namespace DirectX;

class Rigidbody
{
public:
	void Initialize(Vector3 velocity, Vector3 accumulatedForce, float damping, float mass, bool isKinematic);
	void Update(Vector3& transform, float deltaTime);
	void AddForce(Vector3 force);
	void AddForceAt(Vector3 force, Vector3 localPoint, Matrix4x4 worldMatrix);
	void AddForceAt(Vector3 force, Vector3 worldPoint);
	void AddImpulse();
	void ClearAccumulatedForce();
	void InitializeBoxInertiaTensor(Vector3 scale);
	Matrix4x4 inertiaTensor;
	Vector3 orientation = XMQuaternionIdentity();
	Vector3 velocity;// m/s 단위
	Vector3 accumulatedForce;
	float damping;
	float angularDamping;
	float mass;// kg단위
	bool isKinematic;
};