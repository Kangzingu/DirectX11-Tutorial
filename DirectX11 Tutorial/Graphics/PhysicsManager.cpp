#include "Actor.h"
#include "PhysicsManager.h"

void PhysicsManager::Update(vector<Actor*> actors, float deltaTime)
{
	for (int i = 0; i < actors.size(); i++)
	{
		if (isGravityOn)
			actors[i]->rigidbody.AddForce(gravity * actors[i]->rigidbody.GetMass());
		actors[i]->rigidbody.Update(deltaTime);
	}
	// 충돌이 있었다고 치고 해결
	vector<Contact> contacts;
	for (int i = 0; i < actors.size(); i++)
	{
		for (int j = i + 1; j < actors.size(); j++)
		{
			Collision::CubeAndCube(actors[i], actors[j], contacts);
		}
	}

	// 반발계수(일단 임의로 지정함)
	float restitution = 1;
	for (int i = 0; i < contacts.size(); i++)
	{
		// 1. 속도 변경

		float separatingVelocity;
		Vector3 relativeVelocity = contacts[i].object1->rigidbody.GetVelocity() - contacts[i].object2->rigidbody.GetVelocity();
		separatingVelocity = Vector3::Dot(relativeVelocity, contacts[i].normal);

		// 멀어지고 있는거라면
		if (separatingVelocity > 0)
			continue;

		float newSepVelocity = -separatingVelocity * restitution;

		Vector3 accCausedVelocity = contacts[i].object1->rigidbody.GetAccumulatedForce() / contacts[i].object1->rigidbody.GetMass() - contacts[i].object2->rigidbody.GetAccumulatedForce() / contacts[i].object2->rigidbody.GetMass();
		float accCausedSepVelocity = Vector3::Dot(accCausedVelocity, contacts[i].normal) * deltaTime;
		if (accCausedSepVelocity < 0)
		{
			// ? 속도를 음수니까 빼야지 더해지는거 아닌강..
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

		// 2. 겹친 위치 조정
		if (contacts[i].penetration <= 0)
			continue;

		Vector3 movePerIMass = contacts[i].normal * (contacts[i].penetration / totalInverseMass);
		contacts[i].object1->transform.Translate(movePerIMass / contacts[i].object1->rigidbody.GetMass());
		contacts[i].object2->transform.Translate(movePerIMass / -contacts[i].object2->rigidbody.GetMass());
	}
}
