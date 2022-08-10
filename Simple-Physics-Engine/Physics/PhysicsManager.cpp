#include "../Objects/Actor.h"
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
			if (Collision::BroadPhaseBoundingSphere(*(*m_actors)[i], *(*m_actors)[j]) == true)
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
	ResolveVelocity();
}
void PhysicsManager::ResolvePenetration()
{
	Vector3 linearChange[2], angularChange[2];
	Vector3 deltaPosition;
	int index;
	float max;
	float epsilon=0.01;

	int positionIterationsUsed = 0;
	int positionIterations = m_contacts.size();
	while (positionIterationsUsed < positionIterations)
	{
		max = epsilon;
		index = m_contacts.size();
		for (int i = 0; i < m_contacts.size(); i++)
		{
			if (m_contacts[i].m_penetration > max)
			{
				max = m_contacts[i].m_penetration;
				index = i;
			}
		}
		
		if (index == m_contacts.size())// 충돌이 없는 경우
			break;

		m_contacts[index].ModifyPosition(linearChange, angularChange, max);

		for (int i = 0; i < m_contacts.size(); i++)
		{
			for (int b = 0; b < 2; b++) if (m_contacts[i].m_objects[b])
			{
				for (int d = 0; d < 2; d++)
				{
					if (m_contacts[i].m_objects[b] == m_contacts[index].m_objects[d])
					{
						deltaPosition = linearChange[d] + Vector3::Cross(angularChange[d], m_contacts[i].m_relativeContactPosition[b]);
						m_contacts[i].m_penetration += Vector3::Dot(deltaPosition, m_contacts[i].m_normal * (b ? 1 : -1));
					}
				}
			}
		}
		positionIterationsUsed++;
	}
}

void PhysicsManager::ResolveVelocity()
{
	Vector3 velocityChange[2], angularVelocityChange[2];
	Vector3 deltaVelocity;
	int index;
	float max;
	float epsilon = 0.01f;

	int velocityIterationsUsed = 0;
	int velocityIterations = m_contacts.size();
	while (velocityIterationsUsed < velocityIterations)
	{
		max = epsilon;
		index = m_contacts.size();
		for (int i = 0; i < m_contacts.size(); i++)
		{
			if (m_contacts[i].m_desiredDeltaVelocity > max)
			{
				max = m_contacts[i].m_desiredDeltaVelocity;
				index = i;
			}
		}

		if (index == m_contacts.size())// 충돌이 없는 경우
			break;

		m_contacts[index].ModifyVelocity(velocityChange, angularVelocityChange);

		for (int i = 0; i < m_contacts.size(); i++)
		{
			for (int b = 0; b < 2; b++) if (m_contacts[i].m_objects[b])
			{
				for (int d = 0; d < 2; d++)
				{
					if (m_contacts[i].m_objects[b] == m_contacts[index].m_objects[d])
					{
						deltaVelocity = velocityChange[d] + Vector3::Cross(angularVelocityChange[d], m_contacts[i].m_relativeContactPosition[b]);
						m_contacts[i].m_ContactVelocity += m_contacts[i].m_contactToWorld.Transpose() * deltaVelocity * (b ? -1 : 1);
						m_contacts[i].CalculateDesiredDeltaVelocity(m_deltaTime);
					}
				}
			}
		}
		velocityIterationsUsed++;
	}
}