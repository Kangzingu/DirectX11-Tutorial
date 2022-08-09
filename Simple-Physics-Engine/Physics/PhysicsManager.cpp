#include "Actor.h"
#include "PhysicsManager.h"

PhysicsManager::PhysicsManager(vector<Actor*>* actors, float& deltaTime, vector<Vector3>* lineForDebug) : m_actors(actors), m_deltaTime(deltaTime), m_lineForDebug(lineForDebug) {}

void PhysicsManager::Update()
{
	// 순서 계속 헷갈린다.. 마지막에 Transform 을 바꿔주는게 상식적으론 맞는거 같은데 차이를 잘 모르겠다
	GenerateGravity();
	UpdateTransform();
	DetectCollision();
	ResolveCollision();
	m_contacts.clear();
}
void PhysicsManager::GenerateGravity()
{
	for (int i = 0; i < (*m_actors).size(); i++)
	{
		if ((*m_actors)[i]->m_rigidbody.GetInverseMass() != 0)
		{
			(*m_actors)[i]->m_rigidbody.AddForce(m_gravity / (*m_actors)[i]->m_rigidbody.GetInverseMass());
		}
	}
}
void PhysicsManager::UpdateTransform()
{
	for (int i = 0; i < (*m_actors).size(); i++)
	{
		(*m_actors)[i]->m_rigidbody.Update(m_deltaTime);
	}
}
void PhysicsManager::DetectCollision()
{
	for (int i = 0; i < (*m_actors).size(); i++)
	{
		for (int j = i + 1; j < (*m_actors).size(); j++)
		{
			if (Collision::BroadPhaseBoundingSphere(*(*m_actors)[i], *(*m_actors)[j], m_lineForDebug) == true)
				Collision::NarrowPhaseCubeAndCube((*m_actors)[i], (*m_actors)[j], m_contacts, m_lineForDebug);
		}
	}
}
void PhysicsManager::ResolveCollision()
{

	for (int i = 0; i < m_contacts.size(); i++)
	{
		m_contacts[i].CalculateInternals(m_deltaTime);
	}
	ResolvePenetration();


	Vector3 closingVelocity;
	for (int i = 0; i < m_contacts.size(); i++)
	{
		for (int j = 0; j < 2; j++)
		{
			float sign = (j == 0) ? 1 : -1;
			if (m_contacts[i].m_objects[j] != nullptr)
			{
				closingVelocity += m_contacts[i].m_objects[j]->m_rigidbody.GetVelocity() * sign;
			}
		}
		if (Vector3::Dot(closingVelocity, m_contacts[i].m_normal) < 0)
		{
			m_contacts[i].ModifyVelocity();
		}
	}
}

void PhysicsManager::ResolvePenetration()
{
	for (int i = 0; i < m_contacts.size(); i++)
	{
		Vector3 linearChange[2], angularChange[2];
		float max;
		m_contacts[i].ModifyPosition(linearChange, angularChange, m_contacts[i].m_penetration);
	}
	//int index;
	//Vector3 linearChange[2], angularChange[2];
	//float max;
	//float epsilon=0.01;
	//Vector3 deltaPosition;

	//int positionIterationsUsed = 0;
	//int positionIterations = contacts.size();
	//while (positionIterationsUsed < positionIterations)
	//{
	//	max = epsilon;
	//	index = contacts.size();
	//	for (int i = 0; i < contacts.size(); i++)
	//	{
	//		if (contacts[i].m_penetration > max)
	//		{
	//			max = contacts[i].m_penetration;
	//			index = i;
	//		}
	//	}
	//	
	//	if (index == contacts.size())// 충돌이 없는 경우
	//		break;

	//	contacts[index].ModifyPosition(linearChange, angularChange, max);

	//	for (int i = 0; i < contacts.size(); i++)
	//	{
	//		for (int b = 0; b < 2; b++) if (contacts[i].m_objects[b])
	//		{
	//			for (int d = 0; d < 2; d++)
	//			{
	//				if (contacts[i].m_objects[b] == contacts[index].m_objects[d])
	//				{
	//					deltaPosition = linearChange[d] + Vector3::Cross(angularChange[d], contacts[i].m_relativeContactPosition[b]);
	//					contacts[i].m_penetration += Vector3::Dot(deltaPosition, contacts[i].m_normal * (b ? 1 : -1));
	//				}
	//			}
	//		}
	//	}
	//	positionIterationsUsed++;
	//}
}
	
