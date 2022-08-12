#include "Contact.h"
#include "../Objects/Object.h"

void Contact::CalculateInternals(float deltaTime)
{
	CalculateContactToWorldMatrix();
	CalculateRelativeContactPosition();
	CalculateLocalContactVelocity(deltaTime);
	CalculateDesiredDeltaVelocity(deltaTime);
	//CalculateImpulse();
}
void Contact::CalculateRelativeContactPosition()
{
	for (int i = 0; i < 2; i++)
	{
		if (m_objects[i] != nullptr)
		{
			m_relativeContactPosition[i] = m_point - m_objects[i]->m_transform.GetPosition();
		}
	}
}
void Contact::CalculateContactToWorldMatrix()
{
	Vector3 contactTangent[2];
	if (abs(m_normal.x) > abs(m_normal.y))
	{
		contactTangent[0] = Vector3::Up();
		contactTangent[1] = Vector3::Cross(m_normal, contactTangent[0]);
		contactTangent[0] = Vector3::Cross(contactTangent[1], m_normal);
	}
	else
	{
		contactTangent[0] = Vector3::Right();
		contactTangent[1] = Vector3::Cross(m_normal, contactTangent[0]);
		contactTangent[0] = Vector3::Cross(contactTangent[1], m_normal);
	}
	/*if (abs(m_normal.x) > abs(m_normal.y))
	{
		float s = 1.0f / sqrt(m_normal.z * m_normal.z + m_normal.x * m_normal.x);

		contactTangent[0].x = m_normal.z * s;
		contactTangent[0].y = 0;
		contactTangent[0].z = -m_normal.x * s;

		contactTangent[1].x = m_normal.y * contactTangent[0].z;
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
	}*/
	contactTangent[0] = Vector3::Normalize(contactTangent[0]);
	contactTangent[1] = Vector3::Normalize(contactTangent[1]);
	m_contactToWorld = Matrix4x4(m_normal, contactTangent[0], contactTangent[1]);
}
Vector3 Contact::CalculateLocalVelocity(int index, float deltaTime)
{
	// 한 점에서의 속도  Vp = V + (w X r)
	Vector3 velocity = Vector3::Cross(m_objects[index]->m_rigidbody.GetAngularVelocity(), m_relativeContactPosition[index]);
	velocity += m_objects[index]->m_rigidbody.GetVelocity();
	
	// Contact의 좌표계로 변경(normal인 x축 값이 결국 normal(pa - pb)이 되겠징)
	Vector3 contactVelocity = m_contactToWorld.Transpose() * velocity;

	Vector3 accVelocity = m_objects[index]->m_rigidbody.GetLastFrameAcceleration() * deltaTime;
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
			m_contactVelocity += CalculateLocalVelocity(i, deltaTime) * sign;
		}
	}
}
void Contact::CalculateDesiredDeltaVelocity(float deltaTime)
{
	float velocityLimit = 0.25f;

	float velocityFromAcc = 0;

	for (int i = 0; i < 2; i++)
	{
		float sign = (i == 0) ? 1 : -1;
		if (m_objects[i] != nullptr)
		{
			velocityFromAcc += Vector3::Dot(m_objects[i]->m_rigidbody.GetLastFrameAcceleration(), m_normal) * deltaTime * sign;
		}
	}
	float restitution = m_restitution;
	if (abs(m_contactVelocity.x) < velocityLimit)
	{
		restitution = 0;
	}

	/*if (m_desiredDeltaVelocity > 50)
	{
		return;
	}*/
	m_desiredDeltaVelocity = -m_contactVelocity.x -restitution * (m_contactVelocity.x - velocityFromAcc);// -m_ContactVelocity.x - restitution * (m_ContactVelocity.x - velocityFromAcc);
}
Vector3 Contact::CalculateFrictionlessImpulse()
{
	Vector3 deltaVelWorld;
	float deltaVelocity = 0;
	for (int i = 0; i < 2; i++)
	{
		if (m_objects[i] != nullptr)
		{
			deltaVelWorld = Vector3::Cross(m_relativeContactPosition[i], m_normal);
			deltaVelWorld = m_objects[i]->m_rigidbody.GetWorldInertiaTensorInverse() * deltaVelWorld;
			deltaVelWorld = Vector3::Cross(deltaVelWorld, m_relativeContactPosition[i]);
			deltaVelocity += Vector3::Dot(deltaVelWorld, m_normal);
			deltaVelocity += m_objects[i]->m_rigidbody.GetInverseMass();
		}
	}
	return Vector3(m_desiredDeltaVelocity / deltaVelocity, 0, 0);
}
Vector3 Contact::CalculateFrictionImpulse()
{
	Vector4 impulseContact;

	float totalInverseMass = 0;
	Matrix4x4 impulseToTorque;
	Matrix4x4 deltaVelWorld[2];
	Matrix4x4 totalDeltaVelWorld = Matrix4x4::Zero();
	for (int i = 0; i < 2; i++)
	{
		if (m_objects[i] != nullptr)
		{
			impulseToTorque = Matrix4x4::SkewSymmetricForCross(Vector4(m_relativeContactPosition[i], 1));
			deltaVelWorld[i] = impulseToTorque;
			deltaVelWorld[i] *= m_objects[i]->m_rigidbody.GetWorldInertiaTensorInverse();
			deltaVelWorld[i] *= impulseToTorque;
			deltaVelWorld[i] *= -1;
			totalDeltaVelWorld += deltaVelWorld[i];
			totalInverseMass += m_objects[i]->m_rigidbody.GetInverseMass();
		}
	}
	Matrix4x4 deltaVelocity = m_contactToWorld.Transpose();
	deltaVelocity *= totalDeltaVelWorld;
	deltaVelocity *= m_contactToWorld;

	deltaVelocity.m00 += totalInverseMass;
	deltaVelocity.m11 += totalInverseMass;
	deltaVelocity.m22 += totalInverseMass;

	Matrix4x4 impulseMatrix = deltaVelocity.Inverse();
	Vector4 velKill = Vector4(m_desiredDeltaVelocity, -m_contactVelocity.y, -m_contactVelocity.z,1);

	impulseContact = impulseMatrix * velKill;
	
	float planarImpulse = sqrt(impulseContact.y * impulseContact.y + impulseContact.z * impulseContact.z);
	if (planarImpulse > impulseContact.x * m_friction)
	{
		impulseContact.y /= planarImpulse;
		impulseContact.z /= planarImpulse;

		impulseContact.x = 
			deltaVelocity.m00 +
			deltaVelocity.m01 * m_friction * impulseContact.y +
			deltaVelocity.m02 * m_friction * impulseContact.z;
		impulseContact.x = m_desiredDeltaVelocity / impulseContact.x;
		impulseContact.y *= m_friction * impulseContact.x;
		impulseContact.z *= m_friction * impulseContact.x;
	}
	return impulseContact.XYZ();
}
void Contact::ModifyVelocity(Vector3 velocityChange[2], Vector3 angularVelocityChange[2])
{
	Vector3 impulseContact;
	if (m_friction == 0)
	{
		impulseContact =  CalculateFrictionlessImpulse();
	}
	else
	{
        impulseContact =  CalculateFrictionImpulse();
	}
	Vector3 impulse = m_contactToWorld * impulseContact;

	Vector3 impulsiveTorque;
	for (int i = 0; i < 2; i++)
	{
		float sign = (i == 0) ? 1 : -1;
		if (m_objects[i] != nullptr)
		{
			impulsiveTorque = Vector3::Cross(m_relativeContactPosition[i], impulse) * sign;// 찝찝
			angularVelocityChange[i] = m_objects[i]->m_rigidbody.GetWorldInertiaTensorInverse() * impulsiveTorque;
			velocityChange[i] = impulse * m_objects[i]->m_rigidbody.GetInverseMass() * sign;
			m_objects[i]->m_rigidbody.AddVelocity(velocityChange[i]);
			m_objects[i]->m_rigidbody.AddAngularVelocity(angularVelocityChange[i]);
		}
	}
}
void Contact::ModifyPosition(Vector3 linearChange[2], Vector3 angularChange[2], float penetration)
{
	const float angularLimit = 0.2f;
	float angularMove[2];
	float linearMove[2];

	float totalInertia = 0;
	float linearInertia[2];
	float angularInertia[2];

	// 일단 contact 좌표계의 관성 텐서 계산좀 하고
	for (int i = 0; i < 2; i++) if (m_objects[i] != nullptr)
	{
		Vector3 angularInertiaWorld = Vector3::Cross(m_relativeContactPosition[i], m_normal);
		angularInertiaWorld = m_objects[i]->m_rigidbody.GetWorldInertiaTensorInverse() * angularInertiaWorld;
		angularInertiaWorld = Vector3::Cross(angularInertiaWorld, m_relativeContactPosition[i]);
		angularInertia[i] = Vector3::Dot(angularInertiaWorld, m_normal);

		linearInertia[i] = m_objects[i]->m_rigidbody.GetInverseMass();

		totalInertia += linearInertia[i] + angularInertia[i];
	}

	// 계산하고 적용할거임
	for (int i = 0; i < 2; i++) if (m_objects[i] != nullptr)
	{
		float sign = (i == 0) ? 1 : -1;
		angularMove[i] = sign * penetration * (angularInertia[i] / totalInertia);
		linearMove[i] = sign * penetration * (linearInertia[i] / totalInertia);

		Vector3 projection = m_relativeContactPosition[i];
		projection += m_normal * -Vector3::Dot(m_relativeContactPosition[i], m_normal);

		float maxMagnitude = angularLimit * Vector3::Magnitude(projection);
		float totalMove = angularMove[i] + linearMove[i];

		if (angularMove[i] < -maxMagnitude)
		{
			angularMove[i] = -maxMagnitude;
			linearMove[i] = totalMove - angularMove[i];
		}
		else if (angularMove[i] > maxMagnitude)
		{
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
			angularChange[i] = (m_objects[i]->m_rigidbody.GetWorldInertiaTensorInverse() * targetAngularDirection) * (angularMove[i] / angularInertia[i]);
		}

		linearChange[i] = m_normal * linearMove[i];
		m_objects[i]->m_transform.Translate(linearChange[i]);
		m_objects[i]->m_transform.Rotate(angularChange[i]);
	}
}