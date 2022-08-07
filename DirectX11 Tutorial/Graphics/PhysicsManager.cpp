#include "Actor.h"
#include "PhysicsManager.h"

PhysicsManager::PhysicsManager(vector<Actor*>* actors, float& deltaTime, vector<Vector3>* lineForDebug, Vector3* lineColor) : actors(actors), deltaTime(deltaTime), lineForDebug(lineForDebug), lineColor(lineColor) {}

void PhysicsManager::Update()
{
	// 순서 계속 헷갈린다.. 마지막에 Transform 을 바꿔주는게 상식적으론 맞는거 같은데 차이를 잘 모르겠다
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
			// 1. 준비
			contacts[i].CalculateInternals(deltaTime);
			// 2. 속도 변경
			//  2-1. 단위 충격량 당 속도 변화 계산
		}
	}
}
/* 코드 보면거 따라한거*/
//	// 2. 겹친 위치 조정
//{
//	int iter, index;
//	Vector3 linearChange[2], angularChange[2];
//	float max;
//	Vector3 deltaPosition;
//
//	int positionIterationsUsed = 0;
//	int positionIterations = contacts.size() * 4;// 왜 2배인지는..
//	while (positionIterationsUsed < positionIterations)
//	{
//		// 가장 심각한(depth가 깊은)걸 찾음
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
//		// 원래 c[index].matchAwakeState();라는 비활성화된 리지드바디 깨우는듯한?거 함
//
//		// 포지션 변경 할거임
//		const float angularLimit = 0.2f;
//		float angularMove[2];
//		float linearMove[2];
//
//		float totalInertia = 0;
//		float linearInertia[2];
//		float angularInertia[2];
//
//		// 일단 contact 좌표계의 관성 텐서 계산좀 하고
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
//		// 계산하고 적용할거임
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
// 3. 속도 조정
//
//{
//	Vector3 velocityChange[2], rotationChange[2];
//	Vector3 deltaVel;
//
//	int velocityIterationsUsed = 0;
//	int velocityIterations = contacts.size() * 4;// 왜 2배인지는..
//	while (velocityIterationsUsed < velocityIterations)
//	{
//		// 가장 심각한(해결에 필요한 속도가 높은)걸 찾음
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
//		// 원래 c[index].matchAwakeState();라는 비활성화된 리지드바디 깨우는듯한?거 함
//
//		// 속도 변경 할거임
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
//			// 마찰력이 없는 경우
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
//			// 마찰력이 있는 경우
//			// 나중에 구현하자
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

/* 책 보면서 따라한거*/
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
//		// View 변환 시 카메라 기준으로 하기 위해 카메라의 transform의 역행렬을 물체들에게 곱해주듯, contact 기준으로 하려면 contact의 transform에 해당하는 basis행렬의 역행렬을 곱해줘야 함
//		//Matrix4x4 toContactCoordMatrix = contactCoordBasisMatrix.Inverse();
//
//		/* 여기부터 힘 계산 */
//		Vector3 relativeContactPosition = contacts[i].point - contacts[i].object1->transform.GetPosition();// 이거 제대로 초기화 해줘야함 물체 1이 아닌 수 있음
//		Vector3 torquePerUnitImpulse = Vector3::Cross(relativeContactPosition, contacts[i].normal);
//		Vector3 rotationPerUnitImpulse = contacts[i].object1->rigidbody.GetInertiaTensor().Inverse() * torquePerUnitImpulse;
//
//		// 충돌 지점의 각속도를 구한거임(월드 좌표계 기준)
//		Vector3 velocityPerUnitImpulse = Vector3::Cross(rotationPerUnitImpulse, relativeContactPosition);
//
//		// 우리는 Contact 좌표계 기준 각속도가 필요하니 Contact 좌표계로 변환해 줌
//		Vector3 velocityPerUnitImpulseContact = contactToWorldCoordMatrix.Transpose() * velocityPerUnitImpulse;
//
//		// 우리가 Contact Normal을 x축으로 해서 Basis를 구했으니 결국 우리가 원하는 Contact Normal 방향 각속력은 x축 값이 될거임 
//		float angularComponent = velocityPerUnitImpulseContact.x;
//
//
//		/* 여기는 총 합량 계산..? 317pg 참고 */
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
//		/* 여기는 원하는 회전량 계산..? 318pg 참고 */
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
//	// 반발계수(일단 임의로 지정함)
//	float restitution = 0.3f;
//	for (int i = 0; i < contacts.size(); i++)
//	{
//
//		// 1. 속도 변경
//
//		float separatingVelocity;
//		Vector3 relativeVelocity = contacts[i].object1->rigidbody.GetVelocity() - contacts[i].object2->rigidbody.GetVelocity();
//		separatingVelocity = Vector3::Dot(relativeVelocity, contacts[i].normal);
//
//		// 멀어지고 있는거라면
//		if (separatingVelocity > 0)
//			continue;
//
//		float newSepVelocity = -separatingVelocity * restitution;
//
//		Vector3 accCausedVelocity = contacts[i].object1->rigidbody.GetAccumulatedForce() / contacts[i].object1->rigidbody.GetMass() - contacts[i].object2->rigidbody.GetAccumulatedForce() / contacts[i].object2->rigidbody.GetMass();
//		float accCausedSepVelocity = Vector3::Dot(accCausedVelocity, contacts[i].normal) * deltaTime;
//		if (accCausedSepVelocity < 0)
//		{
//			// ? 속도를 음수니까 빼야지 더해지는거 아닌강..
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
//		// 2. 겹친 위치 조정
//		if (contacts[i].depth <= 0)
//			continue;
//
//		totalInverseMass = 1.0f / contacts[i].object1->rigidbody.GetMass() + 1.0f / contacts[i].object2->rigidbody.GetMass();
//		// 질량에 비례해서 떨궈놓는건 수식 유도를 못하겠음.. 아ㅏㅏㅏㅏㅏㅏㅏㅏㅏ
//		Vector3 movePerIMass = contacts[i].normal * (contacts[i].depth / totalInverseMass);
//		contacts[i].object1->transform.Translate(movePerIMass / contacts[i].object1->rigidbody.GetMass());
//		contacts[i].object2->transform.Translate(movePerIMass / -contacts[i].object2->rigidbody.GetMass());
//
//	}
//}
