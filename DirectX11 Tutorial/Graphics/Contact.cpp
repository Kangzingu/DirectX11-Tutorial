#include "Contact.h"
#include "Object.h"

void Contact::CalculateInternals(float deltaTime)
{
	CalculateContactToWorldMatrix();
	CalculateRelativeContactPosition();
	CalculateLocalContactVelocity(deltaTime);
	CalculateResolveSpeed(deltaTime);
}

void Contact::CalculateRelativeContactPosition()
{
	this->relativeContactPosition[0] = point - objects[0]->transform.GetPosition();
	if (objects[1] != nullptr)
	{
		this->relativeContactPosition[1] = point - objects[1]->transform.GetPosition();
	}
}

void Contact::CalculateContactToWorldMatrix()
{
	Vector3 contactTangent[2];
	if (abs(normal.x) > abs(normal.y))
	{
		float s = 1.0f / sqrt(normal.z * normal.z + normal.x * normal.x);

		contactTangent[0].x = normal.z * s;
		contactTangent[0].y = 0;
		contactTangent[0].z = -normal.x * s;

		contactTangent[1].x = normal.y * contactTangent[0].x;
		contactTangent[1].y = normal.z * contactTangent[0].x - normal.x * contactTangent[0].z;
		contactTangent[1].z = -normal.y * contactTangent[0].x;
	}
	else
	{
		float s = 1.0f / sqrt(normal.z * normal.z + normal.y * normal.y);

		contactTangent[0].x = 0;
		contactTangent[0].y = -normal.z * s;
		contactTangent[0].z = normal.y * s;

		contactTangent[1].x = normal.y * contactTangent[0].z - normal.z * contactTangent[0].y;
		contactTangent[1].y = -normal.x * contactTangent[0].z;
		contactTangent[1].z = normal.x * contactTangent[0].y;
	}
	this->contactToWorld = Matrix4x4(normal, contactTangent[0], contactTangent[1]);
}

void Contact::CalculateLocalContactVelocity(float deltaTime)
{
	Vector3 velocity = Vector3::Cross(objects[0]->rigidbody.GetAngularVelocity(), relativeContactPosition[0]);
	velocity += objects[0]->rigidbody.GetVelocity();

	Vector3 contactVelocity = contactToWorld.Transpose() * velocity;
	Vector3 accVelocity = (objects[0]->rigidbody.GetAccumulatedForce() * objects[0]->rigidbody.GetInverseMass()) * deltaTime;
	accVelocity = contactToWorld.Transpose() * accVelocity;
	accVelocity.x = 0;
	contactVelocity += accVelocity;
	if (objects[1] != nullptr)
	{
		velocity = Vector3::Cross(objects[1]->rigidbody.GetAngularVelocity(), relativeContactPosition[1]);
		velocity += objects[1]->rigidbody.GetVelocity();

		contactVelocity -= contactToWorld.Transpose() * velocity;
		accVelocity = (objects[1]->rigidbody.GetAccumulatedForce() * objects[1]->rigidbody.GetInverseMass()) * deltaTime;
		accVelocity = contactToWorld.Transpose() * accVelocity;
		accVelocity.x = 0;
		contactVelocity -= accVelocity;
	}
	this->localContactVelocity = contactVelocity;
}

void Contact::CalculateResolveSpeed(float deltaTime)
{
	const float velocityLimit = 0.25f;
	float velocityFromAcc = 0;

		velocityFromAcc += Vector3::Dot((objects[0]->rigidbody.GetAccumulatedForce() * objects[0]->rigidbody.GetInverseMass()) * deltaTime, normal);

	if (objects[1] != nullptr)
	{
		velocityFromAcc -= Vector3::Dot((objects[1]->rigidbody.GetAccumulatedForce() * objects[1]->rigidbody.GetInverseMass()) * deltaTime, normal);
	}

	if (abs(localContactVelocity.x) < velocityLimit)
	{
		restitution = 0.0f;
	}

	this->resolveSpeed = -localContactVelocity.x - restitution * (localContactVelocity.x - velocityFromAcc);
}