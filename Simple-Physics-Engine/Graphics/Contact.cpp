#include "Contact.h"
#include "Object.h"

void Contact::CalculateInternals(float deltaTime)
{
	CalculateContactToWorldMatrix();
	CalculateRelativeContactPosition();
	Vector3 closingVelocity;
	for (int i = 0; i < 2; i++)
	{
		float sign = (i == 0) ? 1 : -1;
		if (m_objects[i] != nullptr)
		{
			closingVelocity += m_objects[i]->rigidbody.GetVelocity() * sign;
		}
	}
	CalculateLocalContactVelocity(deltaTime);
	CalculateDesiredDeltaVelocity(deltaTime);
	//CalculateImpulse();
	ResolvePosition();
	if (Vector3::Dot(closingVelocity, m_normal) < 0)
	{
		ResolveVelocity();
	}
}

void Contact::CalculateRelativeContactPosition()
{
	for (int i = 0; i < 2; i++)
	{
		if (m_objects[i] != nullptr)
		{
			m_relativeContactPosition[i] = m_point - m_objects[i]->transform.GetPosition();
		}
	}
}

void Contact::CalculateContactToWorldMatrix()
{
	Vector3 contactTangent[2];
	if (abs(m_normal.x) > abs(m_normal.y))
	{
		float s = 1.0f / sqrt(m_normal.z * m_normal.z + m_normal.x * m_normal.x);

		contactTangent[0].x = m_normal.z * s;
		contactTangent[0].y = 0;
		contactTangent[0].z = -m_normal.x * s;

		contactTangent[1].x = m_normal.y * contactTangent[0].x;
		contactTangent[1].y = m_normal.z * contactTangent[0].x - m_normal.x * contactTangent[0].z;
		contactTangent[1].z = -m_normal.y * contactTangent[0].x;
	}
	else
	{
		float s = 1.0f / sqrt(m_normal.z * m_normal.z + m_normal.y * m_normal.y);

		contactTangent[0].x = 0;
		contactTangent[0].y = -m_normal.z * s;
		contactTangent[0].z = m_normal.y * s;

		contactTangent[1].x = m_normal.y * contactTangent[0].z - m_normal.z * contactTangent[0].y;
		contactTangent[1].y = -m_normal.x * contactTangent[0].z;
		contactTangent[1].z = m_normal.x * contactTangent[0].y;
	}
	contactTangent[0] = Vector3::Normalize(contactTangent[0]);
	contactTangent[1] = Vector3::Normalize(contactTangent[1]);
	this->m_contactToWorld = Matrix4x4(m_normal, contactTangent[0], contactTangent[1]);
}

//void Contact::CalculateLocalContactVelocity(float deltaTime)
//{
//	Vector3 velocity = Vector3::Cross(m_objects[0]->rigidbody.GetAngularVelocity(), m_relativeContactPosition[0]);
//	velocity += m_objects[0]->rigidbody.GetVelocity();
//
//	Vector3 contactVelocity = m_contactToWorld.Transpose() * velocity;
//	Vector3 accVelocity = (m_objects[0]->rigidbody.GetAccumulatedForce() * m_objects[0]->rigidbody.GetInverseMass()) * deltaTime;
//	accVelocity = m_contactToWorld.Transpose() * accVelocity;
//	accVelocity.x = 0;
//	contactVelocity += accVelocity;
//	if (m_objects[1] != nullptr)
//	{
//		velocity = Vector3::Cross(m_objects[1]->rigidbody.GetAngularVelocity(), m_relativeContactPosition[1]);
//		velocity += m_objects[1]->rigidbody.GetVelocity();
//
//		contactVelocity -= m_contactToWorld.Transpose() * velocity;
//		accVelocity = (m_objects[1]->rigidbody.GetAccumulatedForce() * m_objects[1]->rigidbody.GetInverseMass()) * deltaTime;
//		accVelocity = m_contactToWorld.Transpose() * accVelocity;
//		accVelocity.x = 0;
//		contactVelocity -= accVelocity;
//	}
//	this->m_localContactVelocity = contactVelocity;
//}
//
//void Contact::CalculateResolveSpeed(float deltaTime)
//{
//	const float velocityLimit = 0.25f;
//	float velocityFromAcc = 0;
//
//		velocityFromAcc += Vector3::Dot((m_objects[0]->rigidbody.GetAccumulatedForce() * m_objects[0]->rigidbody.GetInverseMass()) * deltaTime, m_normal);
//
//	if (m_objects[1] != nullptr)
//	{
//		velocityFromAcc -= Vector3::Dot((m_objects[1]->rigidbody.GetAccumulatedForce() * m_objects[1]->rigidbody.GetInverseMass()) * deltaTime, m_normal);
//	}
//
//	if (abs(m_localContactVelocity.x) < velocityLimit)
//	{
//		m_restitution = 0.0f;
//	}
//
//	this->m_resolveSpeed = -m_localContactVelocity.x - m_restitution * (m_localContactVelocity.x - velocityFromAcc);
//}

Vector3 Contact::CalculateLocalVelocity(int index, float deltaTime)
{
	Vector3 velocity = Vector3::Cross(m_objects[index]->rigidbody.GetAngularVelocity(), m_relativeContactPosition[index]);
	velocity += m_objects[index]->rigidbody.GetVelocity();

	Vector3 contactVelocity = m_contactToWorld.Transpose() * velocity;
	Vector3 accVelocity = m_objects[index]->rigidbody.GetAccumulatedForce() * m_objects[index]->rigidbody.GetInverseMass() * deltaTime;
	accVelocity = m_contactToWorld.Transpose() * accVelocity;

	accVelocity.x = 0;
	contactVelocity += accVelocity;

	return contactVelocity;
}
void Contact::CalculateLocalContactVelocity(float deltaTime)
{
	for (int i = 0; i < 2; i++)
	{
		float sign = (i == 0) ? 1 : -1;
		if (m_objects[i] != nullptr)
		{
			m_ContactVelocity += CalculateLocalVelocity(i, deltaTime) * sign;
		}
	}
}
void Contact::CalculateDesiredDeltaVelocity(float deltaTime)
{
	float velocityLimit = 0.00025f;

	float velocityFromAcc = 0;

	for (int i = 0; i < 2; i++)
	{
		float sign = (i == 0) ? 1 : -1;
		if (m_objects[i] != nullptr)
		{
			velocityFromAcc += Vector3::Dot(m_objects[i]->rigidbody.GetAccumulatedForce() * m_objects[i]->rigidbody.GetInverseMass() * deltaTime, m_normal) * sign;
		}
	}
	float restitution = m_restitution;
	if (abs(m_ContactVelocity.x) < velocityLimit)
	{
		restitution = 0;
	}
	m_desiredDeltaVelocity = -m_ContactVelocity.x - restitution * (m_ContactVelocity.x - velocityFromAcc);
}
Vector3 Contact::CalculateImpulse()
{
	Vector3 deltaVelWorld;
	float deltaVelocity = 0;
	for (int i = 0; i < 2; i++)
	{
		if (m_objects[i] != nullptr)
		{
			deltaVelWorld = Vector3::Cross(m_relativeContactPosition[i], m_normal);
			deltaVelWorld = m_objects[i]->rigidbody.GetWorldInertiaTensorInverse() * deltaVelWorld;
			deltaVelWorld = Vector3::Cross(deltaVelWorld, m_relativeContactPosition[i]);
			deltaVelocity += Vector3::Dot(deltaVelWorld, m_normal);
			deltaVelocity += m_objects[i]->rigidbody.GetInverseMass();
		}
	}
	return Vector3(m_desiredDeltaVelocity / deltaVelocity, 0, 0);
}

void Contact::ResolveVelocity()
{
	Vector3 impulse = m_contactToWorld * CalculateImpulse();
	Vector3 impulsiveTorque;
	Vector3 angularVelocityChange;
	Vector3 linearVelocityChange;
	for (int i = 0; i < 2; i++)
	{
		float sign = (i == 0) ? 1 : -1;
		if (m_objects[i] != nullptr)
		{
			impulsiveTorque = Vector3::Cross(m_relativeContactPosition[i], impulse) * sign;
			angularVelocityChange = m_objects[i]->rigidbody.GetWorldInertiaTensorInverse() * impulsiveTorque;
			linearVelocityChange = impulse * m_objects[i]->rigidbody.GetInverseMass() * sign;
			m_objects[i]->rigidbody.AddVelocity(linearVelocityChange);
			m_objects[i]->rigidbody.AddAngularVelocity(angularVelocityChange);
		}
	}
}

void Contact::ResolvePosition()
{
	const float angularLimit = 0.2f;
	float angularMove[2];
	float linearMove[2];

	float totalInertia = 0;
	float linearInertia[2];
	float angularInertia[2];

	Vector3 angularChange[2], linearChange[2];

	// 일단 contact 좌표계의 관성 텐서 계산좀 하고
	for (int i = 0; i < 2; i++) if (m_objects[i] != nullptr)
	{
		Vector3 angularInertiaWorld = Vector3::Cross(m_relativeContactPosition[i], m_normal);
		angularInertiaWorld = m_objects[i]->rigidbody.GetWorldInertiaTensorInverse() * angularInertiaWorld;
		angularInertiaWorld = Vector3::Cross(angularInertiaWorld, m_relativeContactPosition[i]);
		angularInertia[i] = Vector3::Dot(angularInertiaWorld, m_normal);

		linearInertia[i] = m_objects[i]->rigidbody.GetInverseMass();

		totalInertia += linearInertia[i] + angularInertia[i];
	}

	// 계산하고 적용할거임
	for (int i = 0; i < 2; i++) if (m_objects[i] != nullptr)
	{
		float sign = (i == 0) ? 1 : -1;
		angularMove[i] = sign * m_penetration * (angularInertia[i] / totalInertia);
		linearMove[i] = sign * m_penetration * (linearInertia[i] / totalInertia);

		Vector3 projection = m_relativeContactPosition[i];
		projection += m_normal * -Vector3::Dot(m_relativeContactPosition[i], m_normal);

		float maxMagnitude = angularLimit * Vector3::Magnitude(projection);

		if (angularMove[i] < -maxMagnitude)
		{
			float totalMove = angularMove[i] + linearMove[i];
			angularMove[i] = -maxMagnitude;
			linearMove[i] = totalMove - angularMove[i];
		}
		else if (angularMove[i] > maxMagnitude)
		{
			float totalMove = angularMove[i] + linearMove[i];
			angularMove[i] = maxMagnitude;
			linearMove[i] = totalMove - angularMove[i];
		}

		if (angularMove[i] == 0)
		{
			angularChange[i] = Vector3::Zero();
		}
		else
		{
			Vector3 targetAngularDirection = Vector3::Cross(m_relativeContactPosition[i], m_normal);
			angularChange[i] = m_objects[i]->rigidbody.GetWorldInertiaTensorInverse() * targetAngularDirection * (angularMove[i] / angularInertia[i]);
		}

		linearChange[i] = m_normal * linearMove[i];
		m_objects[i]->transform.Translate(linearChange[i]);
		m_objects[i]->transform.Rotate(angularChange[i]);
	}
}