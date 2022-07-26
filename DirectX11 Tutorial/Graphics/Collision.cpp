#include "Collision.h"
/************************************/
/* 물체-1 이 물체-2 에 가서 박는것! */
/************************************/

void Collision::SphereAndSphere(Transform& t1, Rigidbody& r1, SphereCollider& c1, Transform& t2, Rigidbody& r2, SphereCollider& c2, vector<Contact>& contacts)
{
	Vector3 centerDiff = t1.GetPosition() - t2.GetPosition();
	float centerDistance = Vector3::Magnitude(centerDiff);
	if (centerDistance < c1.radius + c2.radius)
	{
		Contact contact;
		contact.t1 = &t1;
		contact.t2 = &t2;
		contact.point = t2.GetPosition() + (centerDiff / 2.0f);
		contact.normal = Vector3::Normalize(centerDiff);
		contact.penetration = c1.radius + c2.radius - centerDistance;
		contacts.push_back(contact);
	}
}
void Collision::SphereAndPlaneSpace(Transform& t1, Rigidbody& r1, SphereCollider& c1, Transform& t2, Rigidbody& r2, PlaneCollider& c2, vector<Contact>& contacts)
{
	float distance = Vector3::Dot(c2.normal, t1.GetPosition()) - c2.offset - c1.radius;
	// 점과 평면의 거리 공식 + 점이 아니라 구니까 radius도 고려한거임
	if (distance < 0)
	{
		Contact contact;
		contact.t1 = &t1;
		contact.t2 = &t2;
		contact.point = t1.GetPosition() - c2.normal * (c1.radius + distance);
		contact.normal = c2.normal;
		contact.penetration = distance;
		contacts.push_back(contact);
	}
}
void Collision::SphereAndPlane(Transform& t1, Rigidbody& r1, SphereCollider& c1, Transform& t2, Rigidbody& r2, PlaneCollider& c2, vector<Contact>& contacts)
{
	float centerDistance = Vector3::Dot(c2.normal, t1.GetPosition()) - c2.offset;
	// 점과 평면의 거리 공식 + 점이 아니라 구니까 radius도 고려한거임
	if (centerDistance < c1.radius && centerDistance > -c1.radius)
	{
		Contact contact;
		contact.t1 = &t1;
		contact.t2 = &t2;
		contact.point = t1.GetPosition() - c2.normal * centerDistance;
		contact.normal = c2.normal;
		contact.penetration = centerDistance - c1.radius;
		// 평면이 양면일 수 있으니까 양면 다 쳌
		if (centerDistance < 0)
		{
			contact.normal *= -1;
			contact.penetration *= -1;
		}
		contacts.push_back(contact);
	}
}

void Collision::SphereAndCube(Transform& t1, Rigidbody& r1, SphereCollider& c1, Transform& t2, Rigidbody& r2, CubeCollider& c2, vector<Contact>& contacts)
{
	// 큐브의 x, y, z 범위 안에 구가 들어오는지 간단한 확인하기 위해 큐브 기준으로 좌표를 바꿔줌
	Vector3 cubeCoordSphereCenterPosition = t2.GetWorldMatrix().Inverse() * t1.GetPosition();
	Vector3 cubeHalfScale = t2.GetScale() / 2;
	// 큐브의 x, y, z 범위 안에 구가 들어오는지 확인(들어와도 반지름덕에 조금씩 걸친 상태로 충돌이 아닐 수 있어서 함 더 확인해야하긴 함)
	if (abs(cubeCoordSphereCenterPosition.x) - c1.radius < cubeHalfScale.x &&
		abs(cubeCoordSphereCenterPosition.y) - c1.radius < cubeHalfScale.y &&
		abs(cubeCoordSphereCenterPosition.z) - c1.radius < cubeHalfScale.z)
	{
		// 구랑 가장 가까운 점을 구함
		Vector3 closestPoint;
		if (cubeCoordSphereCenterPosition.x > cubeHalfScale.x)
			closestPoint.x = cubeHalfScale.x;
		else if (cubeCoordSphereCenterPosition.x < -cubeHalfScale.x)
			closestPoint.x = -cubeHalfScale.x;
		else
			closestPoint.x = cubeCoordSphereCenterPosition.x;

		if (cubeCoordSphereCenterPosition.y > cubeHalfScale.y)
			closestPoint.y = cubeHalfScale.y;
		else if (cubeCoordSphereCenterPosition.y < -cubeHalfScale.y)
			closestPoint.y = -cubeHalfScale.y;
		else
			closestPoint.y = cubeCoordSphereCenterPosition.y;

		if (cubeCoordSphereCenterPosition.z > cubeHalfScale.z)
			closestPoint.z = cubeHalfScale.z;
		else if (cubeCoordSphereCenterPosition.z < -cubeHalfScale.z)
			closestPoint.z = -cubeHalfScale.z;
		else
			closestPoint.z = cubeCoordSphereCenterPosition.z;
		// 구랑 가장 가까운 점이 반지름 내에 있을 때 겹쳤다고 봄
		if (Vector3::Magnitude(cubeCoordSphereCenterPosition - closestPoint) <= c1.radius)
		{
			Vector3 worldCoordClosestPoint = t2.GetWorldMatrix() * closestPoint;
			Contact contact;
			contact.t1 = &t1;
			contact.t2 = &t2;
			contact.point = worldCoordClosestPoint;
			// 이거 중점을 넘어서 더 겹쳐버리면 계산이 아예 반대로 될듯, 관통해버린걸 해결하는거에 포함해서 해결해야하는 느낌?
			contact.normal = Vector3::Normalize(t1.GetPosition() - worldCoordClosestPoint);
			contact.penetration = c1.radius - Vector3::Magnitude(t1.GetPosition() - worldCoordClosestPoint);
			contacts.push_back(contact);
		}
	}
	// 287pg 읽으면 댐

}

void Collision::CubeAndPlaneSpace(Transform& t1, Rigidbody& r1, CubeCollider& c1, Transform& t2, Rigidbody& r2, PlaneCollider& c2, vector<Contact>& contacts)
{
	Vector3 cubeHalfScale = t1.GetScale() / 2;
	// 큐브의 각 점과 평면의 충돌을 확인함
	Vector3 vertexPositions[8] = {
		t1.GetWorldMatrix() * Vector3(cubeHalfScale.x, cubeHalfScale.y, cubeHalfScale.z),
		t1.GetWorldMatrix() * Vector3(-cubeHalfScale.x, cubeHalfScale.y, cubeHalfScale.z),
		t1.GetWorldMatrix() * Vector3(-cubeHalfScale.x, cubeHalfScale.y, -cubeHalfScale.z),
		t1.GetWorldMatrix() * Vector3(cubeHalfScale.x, cubeHalfScale.y, -cubeHalfScale.z),
		t1.GetWorldMatrix() * Vector3(cubeHalfScale.x, -cubeHalfScale.y, cubeHalfScale.z),
		t1.GetWorldMatrix() * Vector3(-cubeHalfScale.x, -cubeHalfScale.y, cubeHalfScale.z),
		t1.GetWorldMatrix() * Vector3(-cubeHalfScale.x, -cubeHalfScale.y, -cubeHalfScale.z),
		t1.GetWorldMatrix() * Vector3(cubeHalfScale.x, -cubeHalfScale.y, -cubeHalfScale.z)
	};
	for (int i = 0; i < 8; i++)
	{
		// 다음 과정은 구와 평면(단, 반지름이 0) 충돌 확인이랑 같음
		float distance = Vector3::Dot(c2.normal, vertexPositions[i]) - c2.offset;
		if (distance < 0)
		{
			Contact contact;
			contact.t1 = &t1;
			contact.t2 = &t2;
			contact.point = vertexPositions[i] - c2.normal * distance;
			contact.normal = c2.normal;
			contact.penetration = distance;
			contacts.push_back(contact);
		}
	}
}