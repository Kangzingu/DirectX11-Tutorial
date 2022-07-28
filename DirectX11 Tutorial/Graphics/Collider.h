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
//	// 121pg 읽어야함
//public:
//	Actor* actors[2];
//	float restitution;// 반발계수(충돌 깊이의 역할)
//	Vector3 contactNormal;// actor[0]이 actor[1]로 다가가는
//	float penetration;// 충돌 깊이(언젠간 구해야함..ㅎ)
//	float separatingVelocity;
//
//public:
//	void Resolve(float duration)
//	{
//		// 최종적인 접촉 해결 과정은 다음과 같을거임
//		// 1. 각 충돌 별 separating velocity를 구하고 가장 심하게 충돌된 걸 고름
//		// 2. 만약 가장 심한게 0보다 크면(충돌이 아니면) 그냥 끝
//		// 3. 0보다 작으면 그에 대한 해결을 함(Impulse로 속도 바꾸고.. 겹친 부분 떼주고..)
//		// 4. 1단계로 돌아감
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
//		// 두 물체의 상대속력(접촉속력)을 구함
//		separatingVelocity = CalculateSeparatingVelocity();
//
//		if (separatingVelocity > 0)// 접촉이 아니라 멀어지고 있는거라면
//			return;
//
//		// 총 충격량을 구함(충돌 후 운동량 - 충돌 전 운동량)
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
//		// 일케하면 (상대 물체 질량 / 총 질량의 합)따라 영향을 받음(질량이 크면 충격에 따른 속도 변화가 작아져야 하므로)
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
//		// 113pg 참고
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
//{// 충돌 해결을 역으로 생각해서 물체가 멀어지면 가까워지게 하는 식으로, 오히려 연결된 것 처럼 행동하게 해보는거임
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