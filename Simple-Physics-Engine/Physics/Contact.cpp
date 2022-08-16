#include "Contact.h"
#include "../Objects/Object.h"

void Contact::MatchAwakeState()
{
	if (m_objects[1] == nullptr)
		return;

	bool object0IsAwake = m_objects[0]->m_rigidbody.IsAwake();
	bool object1IsAwake = m_objects[1]->m_rigidbody.IsAwake();

	if (object0IsAwake ^ object1IsAwake)
	{
		if (object0IsAwake == true)
			m_objects[1]->m_rigidbody.SetAwake(true);
		else
			m_objects[0]->m_rigidbody.SetAwake(true);
	}
}

void Contact::CalculateInternals(float deltaTime)
{
	if (m_objects[0] == nullptr)
		SwapObjects();

	CalculateContactToWorldMatrix();
	CalculateRelativeContactPosition();
	CalculateLocalContactVelocity(deltaTime);
	CalculateDesiredDeltaVelocity(deltaTime);
}
void Contact::SwapObjects()
{
	m_normal *= -1;
	Object* object;
	object = m_objects[0];
	m_objects[0] = m_objects[1];
	m_objects[1] = object;
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
	contactTangent[0] = Vector3::Normalize(contactTangent[0]);
	contactTangent[1] = Vector3::Normalize(contactTangent[1]);
	m_contactToWorld = Matrix4x4(m_normal, contactTangent[0], contactTangent[1]);
}
Vector3 Contact::CalculateLocalVelocity(int index, float deltaTime)
{
	// �� �������� �ӵ�  Vp = V + (w X r)
	Vector3 pointVelocity = Vector3::Cross(m_objects[index]->m_rigidbody.GetAngularVelocity(), m_relativeContactPosition[index]);
	pointVelocity += m_objects[index]->m_rigidbody.GetVelocity();
	Vector3 contactVelocity = m_contactToWorld.Transpose() * pointVelocity;

	Vector3 accVelocity = m_objects[index]->m_rigidbody.GetLastFrameAcceleration() * deltaTime;
	accVelocity = m_contactToWorld.Transpose() * accVelocity;

	// �ּ��� �߷� ���忡���� y�� ������ �������� �Ŵϱ� ũ�� ���������
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
			m_separateVelocity += CalculateLocalVelocity(i, deltaTime) * sign;
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
			if (m_objects[i]->m_rigidbody.IsAwake() == true)
			{
				velocityFromAcc += Vector3::Dot(m_objects[i]->m_rigidbody.GetLastFrameAcceleration(), m_normal) * deltaTime * sign;
			}
		}
	}
	float restitution = m_restitution;
	if (abs(m_separateVelocity.x) < velocityLimit)
	{
		restitution = 0;
	}

	m_desiredDeltaVelocity = -m_separateVelocity.x -restitution * m_separateVelocity.x - velocityFromAcc;
}
Vector3 Contact::CalculateFrictionlessImpulse()
{
	Vector3 deltaVelocityWorld;
	float deltaVelocity = 0;
	for (int i = 0; i < 2; i++)
	{
		if (m_objects[i] != nullptr)
		{
			deltaVelocityWorld = Vector3::Cross(m_relativeContactPosition[i], m_normal);
			deltaVelocityWorld = m_objects[i]->m_rigidbody.GetWorldInertiaTensorInverse() * deltaVelocityWorld;
			deltaVelocityWorld = Vector3::Cross(deltaVelocityWorld, m_relativeContactPosition[i]);
			deltaVelocity += Vector3::Dot(deltaVelocityWorld, m_normal);
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
	totalDeltaVelWorld.m33 = 1;
	Matrix4x4 deltaVelocity = m_contactToWorld.Transpose();
	deltaVelocity *= totalDeltaVelWorld;
	deltaVelocity *= m_contactToWorld;

	deltaVelocity.m00 += totalInverseMass;
	deltaVelocity.m11 += totalInverseMass;
	deltaVelocity.m22 += totalInverseMass;

	Matrix4x4 impulseMatrix = deltaVelocity.Inverse();

	Vector4 velKill = Vector4(m_desiredDeltaVelocity, 
		-m_separateVelocity.y, 
		-m_separateVelocity.z,
		1);

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
void Contact::ChangeVelocity(Vector3 velocityChange[2], Vector3 angularVelocityChange[2])
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

	Vector3 impulsiveTorque;
	Vector3 impulse = m_contactToWorld * impulseContact;
	for (int i = 0; i < 2; i++)
	{
		float sign = (i == 0) ? 1 : -1;
		if (m_objects[i] != nullptr)
		{
			velocityChange[i] = impulse * m_objects[i]->m_rigidbody.GetInverseMass() * sign;
			impulsiveTorque = Vector3::Cross(m_relativeContactPosition[i], impulse) * sign;
			angularVelocityChange[i] = m_objects[i]->m_rigidbody.GetWorldInertiaTensorInverse() * impulsiveTorque;
			m_objects[i]->m_rigidbody.AddVelocity(velocityChange[i]);
			m_objects[i]->m_rigidbody.AddAngularVelocity(angularVelocityChange[i]);
		}
	}
}
void Contact::ResolvePenetration(Vector3 linearChange[2], Vector3 angularChange[2], float penetration)
{
	const float angularLimit = 0.2f;
	float angularMove[2];
	float linearMove[2];

	float totalInertia = 0;
	float linearInertia[2];
	float angularInertia[2];

	// �ϴ� contact ��ǥ���� ���� �ټ� ����� �ϰ�
	for (int i = 0; i < 2; i++) if (m_objects[i] != nullptr)
	{
		Vector3 angularInertiaWorld = Vector3::Cross(m_relativeContactPosition[i], m_normal);
		angularInertiaWorld = m_objects[i]->m_rigidbody.GetWorldInertiaTensorInverse() * angularInertiaWorld;
		angularInertiaWorld = Vector3::Cross(angularInertiaWorld, m_relativeContactPosition[i]);
		angularInertia[i] = Vector3::Dot(angularInertiaWorld, m_normal);

		linearInertia[i] = m_objects[i]->m_rigidbody.GetInverseMass();

		totalInertia += linearInertia[i] + angularInertia[i];
	}

	// ����ϰ� �����Ұ���
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