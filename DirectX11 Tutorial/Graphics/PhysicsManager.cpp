#include "Actor.h"
#include "PhysicsManager.h"

PhysicsManager::PhysicsManager(vector<Actor*>* actors, float& deltaTime, vector<Vector3>* lineForDebug) : actors(actors), deltaTime(deltaTime), lineForDebug(lineForDebug) {}

void PhysicsManager::Update()
{
	// ���� ��� �򰥸���.. �������� Transform �� �ٲ��ִ°� ��������� �´°� ������ ���̸� �� �𸣰ڴ�
	GenerateGravity();
	UpdateTransform();

	contacts.clear();
	DetectCollision();
	ResolveCollision();
}

void PhysicsManager::GenerateGravity()
{
	for (int i = 0; i < (*actors).size(); i++)
	{
		(*actors)[i]->rigidbody.AddForce(gravity * (*actors)[i]->rigidbody.GetMass());
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
			if (Collision::BroadPhaseBoundingSphere(*(*actors)[i], *(*actors)[j], lineForDebug))
				Collision::NarrowPhaseCubeAndCube((*actors)[i], (*actors)[j], contacts, lineForDebug);
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
	for (int i = 0; i < contacts.size(); i++)
	{
		Vector3 x = contacts[i].normal;
		Vector3 y = Vector3::Up();
		Vector3 z;

		MakeOrthonormalBasis(x, &y, &z);
		Matrix4x4 contactToWorldCoordMatrix = Matrix4x4(x, y, z);
		// View ��ȯ �� ī�޶� �������� �ϱ� ���� ī�޶��� transform�� ������� ��ü�鿡�� �����ֵ�, contact �������� �Ϸ��� contact�� transform�� �ش��ϴ� basis����� ������� ������� ��
		//Matrix4x4 toContactCoordMatrix = contactCoordBasisMatrix.Inverse();
		
		/* ������� �� ��� */
		Vector3 relativeContactPosition = contacts[i].point - contacts[i].object1->transform.GetPosition();// �̰� ����� �ʱ�ȭ ������� ��ü 1�� �ƴ� �� ����
		Vector3 torquePerUnitImpulse = Vector3::Cross(relativeContactPosition, contacts[i].normal);
		Vector3 rotationPerUnitImpulse = contacts[i].object1->rigidbody.GetInertiaTensor().Inverse() * torquePerUnitImpulse;

		// �浹 ������ ���ӵ��� ���Ѱ���(���� ��ǥ�� ����)
		Vector3 velocityPerUnitImpulse = Vector3::Cross(rotationPerUnitImpulse, relativeContactPosition);

		// �츮�� Contact ��ǥ�� ���� ���ӵ��� �ʿ��ϴ� Contact ��ǥ��� ��ȯ�� ��
		Vector3 velocityPerUnitImpulseContact = contactToWorldCoordMatrix.Transpose() * velocityPerUnitImpulse;

		// �츮�� Contact Normal�� x������ �ؼ� Basis�� �������� �ᱹ �츮�� ���ϴ� Contact Normal ���� ���ӷ��� x�� ���� �ɰ��� 
		float angularComponent = velocityPerUnitImpulseContact.x;


		/* ����� �� �շ� ���..? 317pg ���� */
		Vector3 relativeContactPosition1 = contacts[i].point - contacts[i].object1->transform.GetPosition();
		Vector3 deltaVelWorld = Vector3::Cross(relativeContactPosition1, contacts[i].normal);
		deltaVelWorld = contacts[i].object1->transform.GetRotationMatrix()* contacts[i].object1->rigidbody.GetInertiaTensor().Inverse() * contacts[i].object1->transform.GetRotationMatrix().Inverse() * deltaVelWorld;
		deltaVelWorld = Vector3::Cross(deltaVelWorld, relativeContactPosition1);

		float deltaVelocity = Vector3::Dot(deltaVelWorld, contacts[i].normal);

		deltaVelocity += (1.0f / contacts[i].object1->rigidbody.GetMass());

		if (contacts[i].object2 != nullptr)
		{
			Vector3 relativeContactPosition2 = contacts[i].point - contacts[i].object2->transform.GetPosition();
			deltaVelWorld = Vector3::Cross(relativeContactPosition2, contacts[i].normal);
			deltaVelWorld = contacts[i].object2->transform.GetRotationMatrix() * contacts[i].object2->rigidbody.GetInertiaTensor().Inverse() * contacts[i].object2->transform.GetRotationMatrix().Inverse() * deltaVelWorld;
			deltaVelWorld = Vector3::Cross(deltaVelWorld, relativeContactPosition2);

			deltaVelocity += Vector3::Dot(deltaVelWorld, contacts[i].normal);

			deltaVelocity += (1.0f / contacts[i].object2->rigidbody.GetMass());
		}


		/* ����� ���ϴ� ȸ���� ���..? 318pg ���� */
		relativeContactPosition1 = contacts[i].point - contacts[i].object1->transform.GetPosition();

		Vector3 closingVelocity1 = Vector3::Cross(contacts[i].object1->rigidbody.GetAngularVelocity(), relativeContactPosition1);
		closingVelocity1 += contacts[i].object1->rigidbody.GetVelocity();
		if (contacts[i].object2 != nullptr)
		{
			Vector3 relativeContactPosition2 = contacts[i].point - contacts[i].object2->transform.GetPosition();

			Vector3 closingVelocity2 = Vector3::Cross(contacts[i].object2->rigidbody.GetAngularVelocity(), relativeContactPosition2);
			closingVelocity2 += contacts[i].object2->rigidbody.GetVelocity();

			closingVelocity1 += closingVelocity2;
		}
		Vector3 totalClosingVelocity = contactToWorldCoordMatrix.Transpose() * closingVelocity1;
		float restitution = 1.0f;
		float desiredVelocity = -totalClosingVelocity.x * (1 + restitution);

		Vector3 impulseContact = Vector3(desiredVelocity / deltaVelocity, 0, 0);
		Vector3 impulse = contactToWorldCoordMatrix * impulseContact;

		impulse *= -1;
		Vector3 velocityChange = impulse / contacts[i].object1->rigidbody.GetMass();
		Vector3 impulsiveTorque = Vector3::Cross(impulse, relativeContactPosition1);
		Vector3 rotationChange = contacts[i].object1->transform.GetRotationMatrix()* contacts[i].object1->rigidbody.GetInertiaTensor().Inverse()* contacts[i].object1->transform.GetRotationMatrix().Inverse()* impulsiveTorque;

		contacts[i].object1->rigidbody.AddVelocity(velocityChange * deltaTime);
		contacts[i].object1->rigidbody.AddAngularVelocity(rotationChange * deltaTime);
		if (contacts[i].object2 != nullptr)
		{
			Vector3 relativeContactPosition2 = contacts[i].point - contacts[i].object2->transform.GetPosition();

			impulse *= -1;
			velocityChange = impulse / contacts[i].object2->rigidbody.GetMass();
			impulsiveTorque = Vector3::Cross(impulse, relativeContactPosition2);
			rotationChange = contacts[i].object2->transform.GetRotationMatrix() * contacts[i].object2->rigidbody.GetInertiaTensor().Inverse() * contacts[i].object2->transform.GetRotationMatrix().Inverse() * impulsiveTorque;

			contacts[i].object2->rigidbody.AddVelocity(velocityChange * deltaTime);
			contacts[i].object2->rigidbody.AddAngularVelocity(rotationChange * deltaTime);
		}
	}


	// �ݹ߰��(�ϴ� ���Ƿ� ������)
	float restitution = 0.3f;
	for (int i = 0; i < contacts.size(); i++)
	{

		// 1. �ӵ� ����

		float separatingVelocity;
		Vector3 relativeVelocity = contacts[i].object1->rigidbody.GetVelocity() - contacts[i].object2->rigidbody.GetVelocity();
		separatingVelocity = Vector3::Dot(relativeVelocity, contacts[i].normal);

		// �־����� �ִ°Ŷ��
		if (separatingVelocity > 0)
			continue;

		float newSepVelocity = -separatingVelocity * restitution;

		Vector3 accCausedVelocity = contacts[i].object1->rigidbody.GetAccumulatedForce() / contacts[i].object1->rigidbody.GetMass() - contacts[i].object2->rigidbody.GetAccumulatedForce() / contacts[i].object2->rigidbody.GetMass();
		float accCausedSepVelocity = Vector3::Dot(accCausedVelocity, contacts[i].normal) * deltaTime;
		if (accCausedSepVelocity < 0)
		{
			// ? �ӵ��� �����ϱ� ������ �������°� �ƴѰ�..
			newSepVelocity += restitution * accCausedSepVelocity;
			if (newSepVelocity < 0) newSepVelocity = 0;
		}
		float deltaVelocity = newSepVelocity - separatingVelocity;
		float totalInverseMass = 1.0f / contacts[i].object1->rigidbody.GetMass() + 1.0f / contacts[i].object2->rigidbody.GetMass();

		if (totalInverseMass <= 0)
			continue;

		float impulse = deltaVelocity / totalInverseMass;
		Vector3 impulsePerIMess = contacts[i].normal * impulse;
		contacts[i].object1->rigidbody.AddVelocity(impulsePerIMess / contacts[i].object1->rigidbody.GetMass());
		contacts[i].object2->rigidbody.AddVelocity(impulsePerIMess / -contacts[i].object2->rigidbody.GetMass());


		// 2. ��ģ ��ġ ����
		if (contacts[i].depth <= 0)
			continue;

		totalInverseMass = 1.0f / contacts[i].object1->rigidbody.GetMass() + 1.0f / contacts[i].object2->rigidbody.GetMass();
		// ������ ����ؼ� ���ų��°� ���� ������ ���ϰ���.. �Ƥ�����������������
		Vector3 movePerIMass = contacts[i].normal * (contacts[i].depth / totalInverseMass);
		contacts[i].object1->transform.Translate(movePerIMass / contacts[i].object1->rigidbody.GetMass());
		contacts[i].object2->transform.Translate(movePerIMass / -contacts[i].object2->rigidbody.GetMass());

	}
}