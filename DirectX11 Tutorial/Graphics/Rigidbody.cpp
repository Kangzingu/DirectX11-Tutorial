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
	accumulatedForce += force;
}
void Rigidbody::AddForceAt(Vector3 force, Vector3 worldPoint, Transform transform)
{
	accumulatedForce += force;
	accumulatedTorque +=  (Vector3::Cross(force, worldPoint - transform.position));
}
void Rigidbody::AddTorque(Vector3 force, Vector3 worldPoint, Transform transform)
{
	accumulatedTorque += (Vector3::Cross(force, worldPoint - transform.position));
}
void Rigidbody::Update(Transform& transform, float deltaTime)
{
	if (isKinematic) return;
	velocity = (velocity + (accumulatedForce / mass) * deltaTime) * pow(damping, deltaTime);
	transform.position += velocity * deltaTime;
	
	rotationVelocity = (rotationVelocity + (momentOfInertia.Inverse() * accumulatedTorque) * deltaTime) * pow(damping, deltaTime);
	transform.rotation += rotationVelocity * deltaTime;

	ClearAccumulatedForce();
}
void Rigidbody::ClearAccumulatedForce()
{
	accumulatedForce = Vector3::Zero();
	accumulatedTorque = Vector3::Zero();
}