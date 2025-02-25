#include "../Objects/Actor.h"
#include "PhysicsManager.h"

PhysicsManager::PhysicsManager(vector<Actor*>* actors, float& deltaTime, vector<Vector3>* lineForDebug) : m_actors(actors), m_deltaTime(deltaTime), m_lineForDebug(lineForDebug)
{
	for (int i = 0; i < m_actors->size(); i++)
	{
		(*m_actors)[i]->m_rigidbody.SetAcceleration(Vector3(0, -9.8f, 0));
	}
}
void PhysicsManager::Update()
{
	// 授辞 域紗 白哀鍵陥.. 原走厳拭 Transform 聖 郊蚊爽澗惟 雌縦旋生経 限澗暗 旭精汽 託戚研 設 乞牽畏陥
	GenerateGeneralForces();
	UpdateTransform();
	DetectCollision();
	ResolveCollision();
	m_contacts.clear();
}
void PhysicsManager::GenerateGeneralForces()
{
	for (int i = 0; i < (*m_actors).size(); i++)
	{
		if ((*m_actors)[i] == nullptr)
		{
			return;
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
	if (m_contacts.size() == 0)
		return;

	CalculateInternals();
	ResolvePenetration();
	ResolveVelocity();
}
void PhysicsManager::CalculateInternals()
{
	for (int i = 0; i < m_contacts.size(); i++)
	{
		m_contacts[i].CalculateInternals(m_deltaTime);
	}
}
void PhysicsManager::ResolvePenetration()
{
	Vector3 linearChange[2], angularChange[2];
	Vector3 deltaPosition;
	int index;
	float max;

	int positionIterationsUsed = 0;
	int positionIterations = m_contacts.size() * 4;
	while (positionIterationsUsed < positionIterations)
	{
		max = m_epsilon;
		index = m_contacts.size();
		for (int i = 0; i < m_contacts.size(); i++)
		{
			if (m_contacts[i].m_penetration > max)
			{
				max = m_contacts[i].m_penetration;
				index = i;
			}
		}
		
		if (index == m_contacts.size())// 中宜戚 蒸澗 井酔
			break;

		m_contacts[index].MatchAwakeState();

		m_contacts[index].ResolvePenetration(linearChange, angularChange, max);

		for (int i = 0; i < m_contacts.size(); i++)
		{
			for (int b = 0; b < 2; b++) if (m_contacts[i].m_objects[b])
			{
				for (int d = 0; d < 2; d++)
				{
					if (m_contacts[i].m_objects[b] == m_contacts[index].m_objects[d])
					{
						deltaPosition = linearChange[d] + Vector3::Cross(angularChange[d], m_contacts[i].m_relativeContactPosition[b]);
						m_contacts[i].m_penetration += Vector3::Dot(deltaPosition, m_contacts[i].m_normal) * (b ? 1 : -1);
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

	int velocityIterationsUsed = 0;
	int velocityIterations = m_contacts.size() * 4;
	while (velocityIterationsUsed < velocityIterations)
	{
		max = m_epsilon;
		index = m_contacts.size();
		for (int i = 0; i < m_contacts.size(); i++)
		{
			if (m_contacts[i].m_desiredDeltaVelocity > max)
			{
				max = m_contacts[i].m_desiredDeltaVelocity;
				index = i;
			}
		} 

		if (index == m_contacts.size())// 中宜戚 蒸澗 井酔
			break;

		m_contacts[index].MatchAwakeState();

		m_contacts[index].ChangeVelocity(velocityChange, angularVelocityChange);
		//m_contacts[index].CalculateDesiredDeltaVelocity(m_deltaTime);/// 戚暗 隔生艦猿 嬢汗舛亀 照舛鉢鞠澗 汗界昔汽つつつつつつつつつつ 薦企稽吉 戚政澗 乞牽惟魚

		for (int i = 0; i < m_contacts.size(); i++)
		{
			for (int b = 0; b < 2; b++) if (m_contacts[i].m_objects[b])
			{
				for (int d = 0; d < 2; d++)
				{
					if (m_contacts[i].m_objects[b] == m_contacts[index].m_objects[d])
					{
						deltaVelocity = velocityChange[d] + Vector3::Cross(angularVelocityChange[d], m_contacts[i].m_relativeContactPosition[b]);
						m_contacts[i].m_separateVelocity += m_contacts[i].m_contactToWorld.Transpose() * deltaVelocity * (b ? -1 : 1);
						m_contacts[i].CalculateDesiredDeltaVelocity(m_deltaTime);
					}
				}
			}
		}
		velocityIterationsUsed++;
	}
}