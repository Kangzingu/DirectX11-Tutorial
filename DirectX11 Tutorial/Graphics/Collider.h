#pragma once
#include "../Utils/SimpleMath.h"

class Object;
class Collider
{
public:
	Object* object;
	Collider() {}
	void Initialize();
	void Update(float deltaTime);

	bool isEnabled = true;
	Vector3 center = Vector3::Zero();
};

class SphereCollider : public Collider
{
public:
	SphereCollider() {}
	float radius = 1.0f;
};

class CubeCollider : public Collider
{
public:
	CubeCollider() {}
	Vector3 size = Vector3::One();
};

class PlaneCollider : public Collider
{
public:
	PlaneCollider() {}
	Vector3 normal = Vector3::Forward();
	float offset = 0;
};

//class ParticleContact
//{
//	// 121pg �о����
//public:
//	Actor* actors[2];
//	float restitution;// �ݹ߰��(�浹 ������ ����)
//	Vector3 contactNormal;// actor[0]�� actor[1]�� �ٰ�����
//	float penetration;// �浹 ����(������ ���ؾ���..��)
//	float separatingVelocity;
//
//public:
//	void Resolve(float duration)
//	{
//		// �������� ���� �ذ� ������ ������ ��������
//		// 1. �� �浹 �� separating velocity�� ���ϰ� ���� ���ϰ� �浹�� �� ��
//		// 2. ���� ���� ���Ѱ� 0���� ũ��(�浹�� �ƴϸ�) �׳� ��
//		// 3. 0���� ������ �׿� ���� �ذ��� ��(Impulse�� �ӵ� �ٲٰ�.. ��ģ �κ� ���ְ�..)
//		// 4. 1�ܰ�� ���ư�
//		ResolveVelocity(duration);
//		ResolveInterpenetration(duration);
//	}
//	float CalculateSeparatingVelocity() const
//	{
//		Vector3 relativeVelocity = actors[0]->rigidbody.velocity;
//		if (actors[1] != nullptr)
//			relativeVelocity -= actors[1]->rigidbody.velocity;
//		return Vector3::Dot(relativeVelocity, contactNormal);
//	}
//
//private:
//	void ResolveVelocity(float duration)
//	{
//		// �� ��ü�� ���ӷ�(���˼ӷ�)�� ����
//		separatingVelocity = CalculateSeparatingVelocity();
//
//		if (separatingVelocity > 0)// ������ �ƴ϶� �־����� �ִ°Ŷ��
//			return;
//
//		// �� ��ݷ��� ����(�浹 �� ��� - �浹 �� ���)
//		float newSepVelocity = -separatingVelocity * restitution;
//
//		Vector3 accCausedVelocity = actors[0]->rigidbody.accumulatedForce / actors[0]->rigidbody.mass;
//		if (actors[1] != nullptr) accCausedVelocity -= actors[1]->rigidbody.accumulatedForce / actors[1]->rigidbody.mass;
//		float accCausedSepVelocity = Vector3::Dot(accCausedVelocity, contactNormal) * duration;
//		if (accCausedSepVelocity < 0)
//		{
//			newSepVelocity += restitution * accCausedSepVelocity;
//			if (newSepVelocity < 0) newSepVelocity = 0;
//		}
//
//		float deltaVelocity = newSepVelocity - separatingVelocity;
//
//		float totalInverseMass = 1.0f / actors[0]->rigidbody.mass;
//		if (actors[1] != nullptr) totalInverseMass += 1.0f / actors[1]->rigidbody.mass;
//
//		if (totalInverseMass <= 0) return;
//
//		// �����ϸ� (��� ��ü ���� / �� ������ ��)���� ������ ����(������ ũ�� ��ݿ� ���� �ӵ� ��ȭ�� �۾����� �ϹǷ�)
//		float impulse = deltaVelocity / totalInverseMass;
//
//		Vector3 impulsePerIMess = contactNormal * impulse;
//
//		actors[0]->rigidbody.velocity = actors[0]->rigidbody.velocity + impulsePerIMess / actors[0]->rigidbody.mass;
//
//		if (actors[1] != nullptr)
//		{
//			actors[1]->rigidbody.velocity = actors[1]->rigidbody.velocity + impulsePerIMess / -actors[1]->rigidbody.mass;
//		}
//	}
//	void ResolveInterpenetration(float duration)
//	{
//		// 113pg ����
//		if (penetration <= 0) return;
//		float totalInverseMass = 1.0f / actors[0]->rigidbody.mass;
//		if (actors[1] != nullptr) totalInverseMass += 1.0f / actors[1]->rigidbody.mass;
//		if (totalInverseMass <= 0) return;
//		Vector3 movePerIMass = contactNormal * (penetration / totalInverseMass);
//		actors[0]->transform.position += movePerIMass / actors[0]->rigidbody.mass;
//		if (actors[1] != nullptr) actors[1]->transform.position+=movePerIMass / -actors[1]->rigidbody.mass;
//	}
//};
//class ParticleContactResolver
//{
//protected:
//	unsigned int iterations;
//	unsigned int iterationsUsed;
//
//public:
//	ParticleContactResolver(unsigned int iterations)
//	{
//
//	}
//	void SetIterations(unsigned int iterations)
//	{
//
//	}
//	void ResolveContacts(ParticleContact* contactArray, unsigned int numContacts, float duration)
//	{
//		iterationsUsed = 0;
//		while (iterationsUsed < iterations)
//		{
//			float max = 0;
//			unsigned int maxIndex = numContacts;
//			for (unsigned int i = 0; i < numContacts; i++)
//			{
//				float sepVel = contactArray[i].CalculateSeparatingVelocity();
//				if (sepVel < max)
//				{
//					max = sepVel;
//					maxIndex = i;
//				}
//			}
//			contactArray[maxIndex].Resolve(duration);
//			iterationsUsed++;
//		}
//	}
//};
//class ParticleLink
//{// �浹 �ذ��� ������ �����ؼ� ��ü�� �־����� ��������� �ϴ� ������, ������ ����� �� ó�� �ൿ�ϰ� �غ��°���
//public:
//	Actor* actor[2];
//protected:
//	float currentLength() const;
//public:
//	virtual unsigned int fillContact(ParticleContact* contact, unsigned int limit) const = 0;
//	
//};
//class ParticleCable : public ParticleLink
//{
//public:
//	float maxLength;
//	float restitution;
//public:
//	float CurrentLength() const
//	{
//		return Vector3::Magnitude(actor[0]->transform.position - actor[1]->transform.position);
//	}
//	unsigned int fillContact(ParticleContact* contact, unsigned int limit) const
//	{
//		float length = CurrentLength();
//		if (length < maxLength) 
//			return 0;
//
//		contact->actors[0] = actor[0];
//		contact->actors[1] = actor[1];
//
//		contact->contactNormal = Vector3::Normalize(actor[1]->transform.position - actor[0]->transform.position);
//		
//		contact->penetration = length - maxLength;
//		contact->restitution = restitution;
//
//		return 1;
//	}
//};
//ParticleContact contact;