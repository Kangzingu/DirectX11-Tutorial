#include "Actor.h"
#include "PhysicsManager.h"

PhysicsManager::PhysicsManager(vector<Actor*>* actors, float& deltaTime, vector<Vector3>* lineForDebug) : actors(actors), deltaTime(deltaTime), lineForDebug(lineForDebug) {}

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
				Collision::NarrowPhaseCubeAndCube((*actors)[i], (*actors)[j], contacts, lineForDebug);
		}
	}
}
void PhysicsManager::ResolveCollision()
{
	for (int i = 0; i < contacts.size(); i++)
	{
		contacts[i].CalculateInternals(deltaTime);
	}
	ResolvePenetration();

	for (int i = 0; i < contacts.size(); i++)
	{
		contacts[i].ModifyVelocity();
	}
}

void PhysicsManager::ResolvePenetration()
{
	int index;
	Vector3 linearChange[2], angularChange[2];
	float max;
	float epsilon=0.01;
	Vector3 deltaPosition;

	int positionIterationsUsed = 0;
	int positionIterations = contacts.size();
	while (positionIterationsUsed < positionIterations)
	{
		max = epsilon;
		index = contacts.size();
		for (int i = 0; i < contacts.size(); i++)
		{
			if (contacts[i].m_penetration > max)
			{
				max = contacts[i].m_penetration;
				index = i;
			}
		}
		
		if (index == contacts.size())// �浹�� ���� ���
			break;

		contacts[index].ModifyPosition(linearChange, angularChange, max);

		for (int i = 0; i < contacts.size(); i++)
		{
			for (int b = 0; b < 2; b++) if (contacts[i].m_objects[b])
			{
				for (int d = 0; d < 2; d++)
				{
					if (contacts[i].m_objects[b] == contacts[index].m_objects[d])
					{
						deltaPosition = linearChange[d] + Vector3::Cross(angularChange[d], contacts[i].m_relativeContactPosition[b]);
						contacts[i].m_penetration += Vector3::Dot(deltaPosition, contacts[i].m_normal * (b ? 1 : -1));
					}
				}
			}
		}
		positionIterationsUsed++;
	}
	//contacts[i].ModifyPosition();
	//Vector3 closingVelocity;
	//for (int j = 0; j < 2; j++)
	//{
	//	float sign = (j == 0) ? 1 : -1;
	//	if (contacts[i].m_objects[j] != nullptr)
	//	{
	//		closingVelocity += contacts[i].m_objects[j]->rigidbody.GetVelocity() * sign;
	//	}
	//}
}
	
