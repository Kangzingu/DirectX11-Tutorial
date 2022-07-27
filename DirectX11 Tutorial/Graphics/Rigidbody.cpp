#include "Object.h"
#include "Rigidbody.h"

void Rigidbody::Initialize(bool isKinematic, float mass, float damping, float angularDamping, Vector3 velocity, Vector3 rotationVelocity, Matrix4x4 momentOfInertia)
{
	this->isKinematic = isKinematic;
	this->mass = mass;
	this->damping = damping;
	this->angularDamping = angularDamping;
	this->velocity = velocity;
	this->angularVelocity = rotationVelocity;
	this->inertiaTensor = momentOfInertia;
	this->ClearAccumulatedForce();
}
void Rigidbody::AddForce(Vector3 force)
{
	accumulatedForce += force;
}
void Rigidbody::AddForceAt(Vector3 force, Vector3 worldPoint)
{
	accumulatedForce += force;
	accumulatedTorque +=  (Vector3::Cross(worldPoint - object->transform.GetPosition(), force));
}
void Rigidbody::AddTorqueAt(Vector3 force, Vector3 worldPoint)
{
	accumulatedTorque += (Vector3::Cross(worldPoint - object->transform.GetPosition(), force));
}
void Rigidbody::Update(float deltaTime)
{
	if (isKinematic) return;
	velocity = (velocity + (accumulatedForce / mass) * deltaTime) * pow(damping, deltaTime);
	object->transform.Translate(velocity * deltaTime);
	
	angularVelocity = (angularVelocity + (object->transform.GetRotationMatrix() * inertiaTensor.Inverse() * object->transform.GetRotationMatrix().Inverse() * accumulatedTorque) * deltaTime) * pow(angularDamping, deltaTime);
	object->transform.Rotate(angularVelocity * deltaTime);

	ClearAccumulatedForce();
}
void Rigidbody::ClearAccumulatedForce()
{
	accumulatedForce = Vector3::Zero();
	accumulatedTorque = Vector3::Zero();
}