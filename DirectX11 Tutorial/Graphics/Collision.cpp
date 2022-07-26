#include "Collision.h"
/************************************/
/* ��ü-1 �� ��ü-2 �� ���� �ڴ°�! */
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
	// ���� ����� �Ÿ� ���� + ���� �ƴ϶� ���ϱ� radius�� ����Ѱ���
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
	// ���� ����� �Ÿ� ���� + ���� �ƴ϶� ���ϱ� radius�� ����Ѱ���
	if (centerDistance < c1.radius && centerDistance > -c1.radius)
	{
		Contact contact;
		contact.t1 = &t1;
		contact.t2 = &t2;
		contact.point = t1.GetPosition() - c2.normal * centerDistance;
		contact.normal = c2.normal;
		contact.penetration = centerDistance - c1.radius;
		// ����� ����� �� �����ϱ� ��� �� �n
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
	// ť���� x, y, z ���� �ȿ� ���� �������� ������ Ȯ���ϱ� ���� ť�� �������� ��ǥ�� �ٲ���
	Vector3 cubeCoordSphereCenterPosition = t2.GetWorldMatrix().Inverse() * t1.GetPosition();
	Vector3 cubeHalfScale = t2.GetScale() / 2;
	// ť���� x, y, z ���� �ȿ� ���� �������� Ȯ��(���͵� ���������� ���ݾ� ��ģ ���·� �浹�� �ƴ� �� �־ �� �� Ȯ���ؾ��ϱ� ��)
	if (abs(cubeCoordSphereCenterPosition.x) - c1.radius < cubeHalfScale.x &&
		abs(cubeCoordSphereCenterPosition.y) - c1.radius < cubeHalfScale.y &&
		abs(cubeCoordSphereCenterPosition.z) - c1.radius < cubeHalfScale.z)
	{
		// ���� ���� ����� ���� ����
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
		// ���� ���� ����� ���� ������ ���� ���� �� ���ƴٰ� ��
		if (Vector3::Magnitude(cubeCoordSphereCenterPosition - closestPoint) <= c1.radius)
		{
			Vector3 worldCoordClosestPoint = t2.GetWorldMatrix() * closestPoint;
			Contact contact;
			contact.t1 = &t1;
			contact.t2 = &t2;
			contact.point = worldCoordClosestPoint;
			// �̰� ������ �Ѿ �� ���Ĺ����� ����� �ƿ� �ݴ�� �ɵ�, �����ع����� �ذ��ϴ°ſ� �����ؼ� �ذ��ؾ��ϴ� ����?
			contact.normal = Vector3::Normalize(t1.GetPosition() - worldCoordClosestPoint);
			contact.penetration = c1.radius - Vector3::Magnitude(t1.GetPosition() - worldCoordClosestPoint);
			contacts.push_back(contact);
		}
	}
	// 287pg ������ ��

}

void Collision::CubeAndPlaneSpace(Transform& t1, Rigidbody& r1, CubeCollider& c1, Transform& t2, Rigidbody& r2, PlaneCollider& c2, vector<Contact>& contacts)
{
	Vector3 cubeHalfScale = t1.GetScale() / 2;
	// ť���� �� ���� ����� �浹�� Ȯ����
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
		// ���� ������ ���� ���(��, �������� 0) �浹 Ȯ���̶� ����
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