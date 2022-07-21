#include "Rigidbody.h"

void Rigidbody::Initialize(Vector3 velocity, Vector3 accumulatedForce, float damping, float mass, bool isKinematic)
{
	this->velocity = velocity;
	this->accumulatedForce = accumulatedForce;
	this->damping = damping;
	this->mass = mass;
	this->isKinematic = isKinematic;
}
void Rigidbody::AddForce(Vector3 force)
{
	this->accumulatedForce += force;
}
void Rigidbody::AddForceAt(Vector3 force, Vector3 localPoint, Matrix4x4 worldMatrix)
{
	AddForceAt(force, worldMatrix * localPoint);
}
void Rigidbody::AddForceAt(Vector3 force, Vector3 worldPoint)
{

}
void Rigidbody::AddImpulse()
{

}
void Rigidbody::InitializeBoxInertiaTensor(Vector3 scale)
{
	inertiaTensor = XMMatrixSet(1.0f / 12.0f * mass * (pow(scale.y, 2) + pow(scale.z, 2)), 0, 0, 0,
								0, 1.0f / 12.0f * mass * (pow(scale.x, 2) + pow(scale.z, 2)), 0, 0,
								0, 0, 1.0f / 12.0f * mass * (pow(scale.x, 2) + pow(scale.y, 2)), 0,
								0, 0, 0, 1);
}
void Rigidbody::Update(Transform& transform, float deltaTime)
{
	if (isKinematic) return;
	velocity = (velocity + (accumulatedForce / mass) * deltaTime) * pow(damping, deltaTime);
	transform.position += velocity * deltaTime;
	ClearAccumulatedForce();
}
void Rigidbody::ClearAccumulatedForce()
{
	accumulatedForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}