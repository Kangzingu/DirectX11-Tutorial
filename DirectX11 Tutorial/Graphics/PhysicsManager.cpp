#include "Actor.h"
#include "PhysicsManager.h"

PhysicsManager::PhysicsManager(vector<Actor*>* actors, float& deltaTime, vector<Vector3>* lineForDebug, Vector3* lineColor) : actors(actors), deltaTime(deltaTime), lineForDebug(lineForDebug), lineColor(lineColor) {}

void PhysicsManager::Update()
{
	// ���� ��� �򰥸���.. �������� Transform �� �ٲ��ִ°� ��������� �´°� ������ ���̸� �� �𸣰ڴ�
	GenerateGravity();
	UpdateTransform();

	DetectCollision();
	ResolveCollision();
	contacts.clear();
}

void PhysicsManager::GenerateGravity()
{
	for (int i = 0; i < (*actors).size(); i++)
	{
		if ((*actors)[i]->rigidbody.GetInverseMass() != 0)
		{
			(*actors)[i]->rigidbody.AddForce(gravity / (*actors)[i]->rigidbody.GetInverseMass());
		}
	}
}

void PhysicsManager::UpdateTransform()
{
	for (int i = 0; i < (*actors).size(); i++)
	{
		(*actors)[i]->rigidbody.Update(deltaTime);
	}
}

void PhysicsManager::DetectCollision()
{
	for (int i = 0; i < (*actors).size(); i++)
	{
		for (int j = i + 1; j < (*actors).size(); j++)
		{
			if (Collision::BroadPhaseBoundingSphere(*(*actors)[i], *(*actors)[j], lineForDebug) == true)
			//{
				Collision::NarrowPhaseCubeAndCube((*actors)[i], (*actors)[j], contacts, lineForDebug);
				/*if (Collision::SATTest(*(*actors)[i], *(*actors)[j], lineForDebug) == true)
				{
					*lineColor = Vector3(1, 0, 0);
				}
				else
				{
					*lineColor = Vector3(0, 1, 0);
				}*/
			//}
		}
	}
}

void MakeOrthonormalBasis(const Vector3& x, Vector3* y, Vector3* z)
{
	*z = Vector3::Cross(x, *y);
	if (Vector3::SquareMagnitude(*z) == 0)
	{
		*y = Vector3(y->y, y->z, y->x);
		*z = Vector3::Cross(x, *y);
	}
	*y = Vector3::Cross(*z, x);

	*z = Vector3::Normalize(*z);
	*y = Vector3::Normalize(*y);
}

void PhysicsManager::ResolveCollision()
{
	{
		for (int i = 0; i < contacts.size(); i++)
		{
			// 1. �غ�
			contacts[i].CalculateInternals(deltaTime);
			// 2. �ӵ� ����
			//  2-1. ���� ��ݷ� �� �ӵ� ��ȭ ���
		}
	}
}
/* �ڵ� ����� �����Ѱ�*/
//	// 2. ��ģ ��ġ ����
//{
//	int iter, index;
//	Vector3 linearChange[2], angularChange[2];
//	float max;
//	Vector3 deltaPosition;
//
//	int positionIterationsUsed = 0;
//	int positionIterations = contacts.size() * 4;// �� 2��������..
//	while (positionIterationsUsed < positionIterations)
//	{
//		// ���� �ɰ���(depth�� ����)�� ã��
//		max = epsilon;
//		index = contacts.size();
//		for (int i = 0; i < contacts.size(); i++)
//		{
//			if (contacts[i].depth > max)
//			{
//				max = contacts[i].depth;
//				index = i;
//			}
//		}
//		if (index == contacts.size())
//			break;
//
//		// ���� c[index].matchAwakeState();��� ��Ȱ��ȭ�� ������ٵ� ����µ���?�� ��
//
//		// ������ ���� �Ұ���
//		const float angularLimit = 0.2f;
//		float angularMove[2];
//		float linearMove[2];
//
//		float totalInertia = 0;
//		float linearInertia[2];
//		float angularInertia[2];
//
//		// �ϴ� contact ��ǥ���� ���� �ټ� ����� �ϰ�
//		for (int i = 0; i < 2; i++) if (contacts[index].objects[i] != nullptr)
//		{
//			Matrix4x4 inverseInertiaTensor = contacts[index].objects[i]->transform.GetWorldMatrix() * contacts[index].objects[i]->rigidbody.GetInertiaTensor().Inverse() * contacts[index].objects[i]->transform.GetWorldMatrix().Inverse();
//
//			Vector3 angularInertiaWorld = Vector3::Cross(contacts[index].relativeContactPosition[i], contacts[index].normal);
//			angularInertiaWorld = inverseInertiaTensor * angularInertiaWorld;
//			angularInertiaWorld = Vector3::Cross(angularInertiaWorld, contacts[index].relativeContactPosition[i]);
//			angularInertia[i] = Vector3::Dot(angularInertiaWorld, contacts[index].normal);
//
//			linearInertia[i] = contacts[index].objects[i]->rigidbody.GetInverseMass();
//
//			totalInertia += linearInertia[i] + angularInertia[i];
//		}
//
//		// ����ϰ� �����Ұ���
//		for (int i = 0; i < 2; i++) if (contacts[index].objects[i] != nullptr)
//		{
//			float sign = (i == 0) ? 1 : -1;
//			angularMove[i] = sign * contacts[index].depth * (angularInertia[i] / totalInertia);
//			linearMove[i] = sign * contacts[index].depth * (linearInertia[i] / totalInertia);
//
//			Vector3 projection = contacts[index].relativeContactPosition[i];
//			projection += contacts[index].normal * Vector3::Dot(-contacts[index].relativeContactPosition[i], contacts[index].normal);
//
//			float maxMagnitude = angularLimit * Vector3::Magnitude(projection);
//
//			if (angularMove[i] < -maxMagnitude)
//			{
//				float totalMove = angularMove[i] + linearMove[i];
//				angularMove[i] = -maxMagnitude;
//				linearMove[i] = totalMove - angularMove[i];
//			}
//			else if (angularMove[i] > maxMagnitude)
//			{
//				float totalMove = angularMove[i] + linearMove[i];
//				angularMove[i] = maxMagnitude;
//				linearMove[i] = totalMove - angularMove[i];
//			}
//
//			if (angularMove[i] == 0)
//			{
//				angularChange[i] = Vector3::Zero();
//			}
//			else
//			{
//				Vector3 targetAngularDirection = Vector3::Cross(contacts[index].relativeContactPosition[i], contacts[index].normal);
//				Matrix4x4 inverseInertiaTensor = contacts[index].objects[i]->transform.GetWorldMatrix() * contacts[index].objects[i]->rigidbody.GetInertiaTensor().Inverse() * contacts[index].objects[i]->transform.GetWorldMatrix().Inverse();
//				angularChange[i] = inverseInertiaTensor * targetAngularDirection * (angularMove[i] / angularInertia[i]);
//			}
//
//			linearChange[i] = contacts[index].normal * linearMove[i];
//			contacts[index].objects[i]->transform.Translate(contacts[index].normal * linearMove[i]);
//			contacts[index].objects[i]->transform.Rotate(angularChange[i]);
//		}
//		for (int i = 0; i < contacts.size(); i++)
//		{
//			for (int a = 0; a < 2; a++) if (contacts[i].objects[a] != nullptr)
//			{
//				for (int b = 0; b < 2; b++)
//				{
//					if (contacts[i].objects[a] == contacts[index].objects[b])
//					{
//						deltaPosition = linearChange[b] + Vector3::Cross(angularChange[b], contacts[i].relativeContactPosition[a]);
//						contacts[i].depth += Vector3::Dot(deltaPosition, contacts[i].normal) * (a ? 1 : -1);
//					}
//				}
//			}
//		}
//		positionIterationsUsed++;
//	}
//}
//
// 3. �ӵ� ����
//
//{
//	Vector3 velocityChange[2], rotationChange[2];
//	Vector3 deltaVel;
//
//	int velocityIterationsUsed = 0;
//	int velocityIterations = contacts.size() * 4;// �� 2��������..
//	while (velocityIterationsUsed < velocityIterations)
//	{
//		// ���� �ɰ���(�ذῡ �ʿ��� �ӵ��� ����)�� ã��
//		float max = epsilon;
//		int index = contacts.size();
//		for (int i = 0; i < contacts.size(); i++)
//		{
//			if (contacts[i].resolveSpeed > max)
//			{
//				max = contacts[i].resolveSpeed;
//				index = i;
//			}
//		}
//		if (index == contacts.size())
//			break;
//
//		// ���� c[index].matchAwakeState();��� ��Ȱ��ȭ�� ������ٵ� ����µ���?�� ��
//
//		// �ӵ� ���� �Ұ���
//		Matrix4x4 inverseInertiaTensor[2];
//		inverseInertiaTensor[0] = contacts[index].objects[0]->transform.GetWorldMatrix() * contacts[index].objects[0]->rigidbody.GetInertiaTensor().Inverse() * contacts[index].objects[0]->transform.GetWorldMatrix().Inverse();
//		if (contacts[index].objects[1] != nullptr)
//		{
//			inverseInertiaTensor[1] = contacts[index].objects[1]->transform.GetWorldMatrix() * contacts[index].objects[1]->rigidbody.GetInertiaTensor().Inverse() * contacts[index].objects[1]->transform.GetWorldMatrix().Inverse();
//		}
//
//		Vector3 impulseContact;
//		float friction = 0;
//		if (friction == 0)
//		{
//			// �������� ���� ���
//			Vector3 deltaVelWorld = Vector3::Cross(contacts[index].relativeContactPosition[0], contacts[index].normal);
//			deltaVelWorld = inverseInertiaTensor[0] * deltaVelWorld;
//			deltaVelWorld = Vector3::Cross(deltaVelWorld, contacts[index].relativeContactPosition[0]);
//
//			float deltaVelocity = Vector3::Dot(deltaVelWorld, contacts[index].normal);
//
//			deltaVelocity +=  contacts[index].objects[0]->rigidbody.GetInverseMass();
//			if (contacts[index].objects[1] != nullptr)
//			{
//				deltaVelWorld = Vector3::Cross(contacts[index].relativeContactPosition[1], contacts[index].normal);
//				deltaVelWorld = inverseInertiaTensor[1] * deltaVelWorld;
//				deltaVelWorld = Vector3::Cross(deltaVelWorld, contacts[index].relativeContactPosition[1]);
//
//				deltaVelocity += Vector3::Dot(deltaVelWorld, contacts[index].normal);
//
//				deltaVelocity += contacts[index].objects[1]->rigidbody.GetInverseMass();
//			}
//			impulseContact.x = contacts[index].resolveSpeed / deltaVelocity;
//		}
//		else
//		{
//			// �������� �ִ� ���
//			// ���߿� ��������
//		}
//		Vector3 impulse = contacts[index].contactToWorld * impulseContact;
//
//		Vector3 impulsiveTorque = Vector3::Cross(contacts[index].relativeContactPosition[0], impulse);
//		rotationChange[0] = inverseInertiaTensor[0] * impulsiveTorque;
//		velocityChange[0] = impulse * contacts[index].objects[0]->rigidbody.GetInverseMass();
//
//		contacts[index].objects[0]->transform.Translate(velocityChange[0]);
//		contacts[index].objects[0]->transform.Rotate(rotationChange[0]);
//		if (contacts[index].objects[1] != nullptr)
//		{
//			impulsiveTorque = Vector3::Cross(impulse, contacts[index].relativeContactPosition[1]);
//			rotationChange[1] = inverseInertiaTensor[1] * impulsiveTorque;
//			velocityChange[1] = -impulse * contacts[index].objects[1]->rigidbody.GetInverseMass();
//
//			contacts[index].objects[1]->transform.Translate(velocityChange[1]);
//			contacts[index].objects[1]->transform.Rotate(rotationChange[1]);
//		}
//
//		for (int i = 0; i < contacts.size(); i++)
//		{
//			for (int a = 0; a < 2; a++) if (contacts[index].objects[a] != nullptr)
//			{
//				for (int b = 0; b < 2; b++)
//				{
//					if (contacts[i].objects[a] == contacts[index].objects[b])
//					{
//						deltaVel = velocityChange[b] + Vector3::Cross(rotationChange[b], contacts[i].relativeContactPosition[a]);
//						contacts[i].localContactVelocity += contacts[i].contactToWorld.Transpose() * deltaVel * (a ? 1 : -1);
//						contacts[i].CalculateResolveSpeed(deltaTime);
//					}
//
//				}
//			}
//		}
//		velocityIterationsUsed++;
//	}
//}

/* å ���鼭 �����Ѱ�*/
//void PhysicsManager::ResolveCollision()
//{
//	for (int i = 0; i < contacts.size(); i++)
//	{
//		Vector3 x = contacts[i].normal;
//		Vector3 y = Vector3::Up();
//		Vector3 z;
//
//		MakeOrthonormalBasis(x, &y, &z);
//		Matrix4x4 contactToWorldCoordMatrix = Matrix4x4(x, y, z);
//		// View ��ȯ �� ī�޶� �������� �ϱ� ���� ī�޶��� transform�� ������� ��ü�鿡�� �����ֵ�, contact �������� �Ϸ��� contact�� transform�� �ش��ϴ� basis����� ������� ������� ��
//		//Matrix4x4 toContactCoordMatrix = contactCoordBasisMatrix.Inverse();
//
//		/* ������� �� ��� */
//		Vector3 relativeContactPosition = contacts[i].point - contacts[i].object1->transform.GetPosition();// �̰� ����� �ʱ�ȭ ������� ��ü 1�� �ƴ� �� ����
//		Vector3 torquePerUnitImpulse = Vector3::Cross(relativeContactPosition, contacts[i].normal);
//		Vector3 rotationPerUnitImpulse = contacts[i].object1->rigidbody.GetInertiaTensor().Inverse() * torquePerUnitImpulse;
//
//		// �浹 ������ ���ӵ��� ���Ѱ���(���� ��ǥ�� ����)
//		Vector3 velocityPerUnitImpulse = Vector3::Cross(rotationPerUnitImpulse, relativeContactPosition);
//
//		// �츮�� Contact ��ǥ�� ���� ���ӵ��� �ʿ��ϴ� Contact ��ǥ��� ��ȯ�� ��
//		Vector3 velocityPerUnitImpulseContact = contactToWorldCoordMatrix.Transpose() * velocityPerUnitImpulse;
//
//		// �츮�� Contact Normal�� x������ �ؼ� Basis�� �������� �ᱹ �츮�� ���ϴ� Contact Normal ���� ���ӷ��� x�� ���� �ɰ��� 
//		float angularComponent = velocityPerUnitImpulseContact.x;
//
//
//		/* ����� �� �շ� ���..? 317pg ���� */
//		Vector3 relativeContactPosition1 = contacts[i].point - contacts[i].object1->transform.GetPosition();
//		Vector3 deltaVelWorld = Vector3::Cross(relativeContactPosition1, contacts[i].normal);
//		deltaVelWorld = contacts[i].object1->transform.GetRotationMatrix() * contacts[i].object1->rigidbody.GetInertiaTensor().Inverse() * contacts[i].object1->transform.GetRotationMatrix().Inverse() * deltaVelWorld;
//		deltaVelWorld = Vector3::Cross(deltaVelWorld, relativeContactPosition1);
//
//		float deltaVelocity = Vector3::Dot(deltaVelWorld, contacts[i].normal);
//
//		deltaVelocity += (1.0f / contacts[i].object1->rigidbody.GetMass());
//
//		if (contacts[i].object2 != nullptr)
//		{
//			Vector3 relativeContactPosition2 = contacts[i].point - contacts[i].object2->transform.GetPosition();
//			deltaVelWorld = Vector3::Cross(relativeContactPosition2, contacts[i].normal);
//			deltaVelWorld = contacts[i].object2->transform.GetRotationMatrix() * contacts[i].object2->rigidbody.GetInertiaTensor().Inverse() * contacts[i].object2->transform.GetRotationMatrix().Inverse() * deltaVelWorld;
//			deltaVelWorld = Vector3::Cross(deltaVelWorld, relativeContactPosition2);
//
//			deltaVelocity += Vector3::Dot(deltaVelWorld, contacts[i].normal);
//
//			deltaVelocity += (1.0f / contacts[i].object2->rigidbody.GetMass());
//		}
//
//
//		/* ����� ���ϴ� ȸ���� ���..? 318pg ���� */
//		relativeContactPosition1 = contacts[i].point - contacts[i].object1->transform.GetPosition();
//
//		Vector3 closingVelocity1 = Vector3::Cross(contacts[i].object1->rigidbody.GetAngularVelocity(), relativeContactPosition1);
//		closingVelocity1 += contacts[i].object1->rigidbody.GetVelocity();
//		if (contacts[i].object2 != nullptr)
//		{
//			Vector3 relativeContactPosition2 = contacts[i].point - contacts[i].object2->transform.GetPosition();
//
//			Vector3 closingVelocity2 = Vector3::Cross(contacts[i].object2->rigidbody.GetAngularVelocity(), relativeContactPosition2);
//			closingVelocity2 += contacts[i].object2->rigidbody.GetVelocity();
//
//			closingVelocity1 += closingVelocity2;
//		}
//		Vector3 totalClosingVelocity = contactToWorldCoordMatrix.Transpose() * closingVelocity1;
//		float restitution = 1.0f;
//		float desiredVelocity = -totalClosingVelocity.x * (1 + restitution);
//
//		Vector3 impulseContact = Vector3(desiredVelocity / deltaVelocity, 0, 0);
//		Vector3 impulse = contactToWorldCoordMatrix * impulseContact;
//
//		impulse *= -1;
//		Vector3 velocityChange = impulse / contacts[i].object1->rigidbody.GetMass();
//		Vector3 impulsiveTorque = Vector3::Cross(impulse, relativeContactPosition1);
//		Vector3 rotationChange = contacts[i].object1->transform.GetRotationMatrix() * contacts[i].object1->rigidbody.GetInertiaTensor().Inverse() * contacts[i].object1->transform.GetRotationMatrix().Inverse() * impulsiveTorque;
//
//		contacts[i].object1->rigidbody.AddVelocity(velocityChange * deltaTime);
//		contacts[i].object1->rigidbody.AddAngularVelocity(rotationChange * deltaTime);
//		if (contacts[i].object2 != nullptr)
//		{
//			Vector3 relativeContactPosition2 = contacts[i].point - contacts[i].object2->transform.GetPosition();
//
//			impulse *= -1;
//			velocityChange = impulse / contacts[i].object2->rigidbody.GetMass();
//			impulsiveTorque = Vector3::Cross(impulse, relativeContactPosition2);
//			rotationChange = contacts[i].object2->transform.GetRotationMatrix() * contacts[i].object2->rigidbody.GetInertiaTensor().Inverse() * contacts[i].object2->transform.GetRotationMatrix().Inverse() * impulsiveTorque;
//
//			contacts[i].object2->rigidbody.AddVelocity(velocityChange * deltaTime);
//			contacts[i].object2->rigidbody.AddAngularVelocity(rotationChange * deltaTime);
//		}
//	}
//
//
//	// �ݹ߰��(�ϴ� ���Ƿ� ������)
//	float restitution = 0.3f;
//	for (int i = 0; i < contacts.size(); i++)
//	{
//
//		// 1. �ӵ� ����
//
//		float separatingVelocity;
//		Vector3 relativeVelocity = contacts[i].object1->rigidbody.GetVelocity() - contacts[i].object2->rigidbody.GetVelocity();
//		separatingVelocity = Vector3::Dot(relativeVelocity, contacts[i].normal);
//
//		// �־����� �ִ°Ŷ��
//		if (separatingVelocity > 0)
//			continue;
//
//		float newSepVelocity = -separatingVelocity * restitution;
//
//		Vector3 accCausedVelocity = contacts[i].object1->rigidbody.GetAccumulatedForce() / contacts[i].object1->rigidbody.GetMass() - contacts[i].object2->rigidbody.GetAccumulatedForce() / contacts[i].object2->rigidbody.GetMass();
//		float accCausedSepVelocity = Vector3::Dot(accCausedVelocity, contacts[i].normal) * deltaTime;
//		if (accCausedSepVelocity < 0)
//		{
//			// ? �ӵ��� �����ϱ� ������ �������°� �ƴѰ�..
//			newSepVelocity += restitution * accCausedSepVelocity;
//			if (newSepVelocity < 0) newSepVelocity = 0;
//		}
//		float deltaVelocity = newSepVelocity - separatingVelocity;
//		float totalInverseMass = 1.0f / contacts[i].object1->rigidbody.GetMass() + 1.0f / contacts[i].object2->rigidbody.GetMass();
//
//		if (totalInverseMass <= 0)
//			continue;
//
//		float impulse = deltaVelocity / totalInverseMass;
//		Vector3 impulsePerIMess = contacts[i].normal * impulse;
//		contacts[i].object1->rigidbody.AddVelocity(impulsePerIMess / contacts[i].object1->rigidbody.GetMass());
//		contacts[i].object2->rigidbody.AddVelocity(impulsePerIMess / -contacts[i].object2->rigidbody.GetMass());
//
//
//		// 2. ��ģ ��ġ ����
//		if (contacts[i].depth <= 0)
//			continue;
//
//		totalInverseMass = 1.0f / contacts[i].object1->rigidbody.GetMass() + 1.0f / contacts[i].object2->rigidbody.GetMass();
//		// ������ ����ؼ� ���ų��°� ���� ������ ���ϰ���.. �Ƥ�����������������
//		Vector3 movePerIMass = contacts[i].normal * (contacts[i].depth / totalInverseMass);
//		contacts[i].object1->transform.Translate(movePerIMass / contacts[i].object1->rigidbody.GetMass());
//		contacts[i].object2->transform.Translate(movePerIMass / -contacts[i].object2->rigidbody.GetMass());
//
//	}
//}
