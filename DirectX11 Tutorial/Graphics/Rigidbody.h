#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class Rigidbody
{
public:
	void Initialize(XMVECTOR velocity, XMVECTOR accumulatedForce, float damping, float mass, bool isKinematic)
	{
		this->velocity = velocity;
		this->accumulatedForce = accumulatedForce;
		this->damping = damping;
		this->mass = mass;
		this->isKinematic = isKinematic;
	}
	void AddForce(XMVECTOR force)
	{
		this->accumulatedForce += force;
	}
	XMVECTOR velocity;// m/s 단위
	XMVECTOR accumulatedForce;
	float damping;
	float mass;// kg단위
	bool isKinematic;
};