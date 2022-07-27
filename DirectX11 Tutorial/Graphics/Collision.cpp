#include "Object.h"
#include "Collision.h"
/************************************/
/* ��ü-1 �� ��ü-2 �� ���� �ڴ°�! */
/************************************/

void Collision::LayAndSphere(Vector3& layPosition, Vector3& layDirection, Object* object)
{
	Vector3 layToSphere = object->transform.GetPosition() - layPosition;
	// lay(ī�޶�)�� ������ �� �ȿ� �ִ� ���. ��Ȳ�� ���� �˾Ƽ� ó���ϵ���..
	if (Vector3::SquareMagnitude(layToSphere) < object->sphereCollider.radius * object->sphereCollider.radius)
	{
	}
	float layToSphereDotLayDirectionResult = Vector3::Dot(layToSphere, layDirection);
	// lay(ī�޶�)�� ������⿡ ���� �ִ� ���. �ĸ鿡 ���� ������ 1�������� �ɷ���
	if (layToSphereDotLayDirectionResult >= 0)
	{
		float layAndSphereLeastLengthSquare = Vector3::SquareMagnitude(layToSphere) - layToSphereDotLayDirectionResult * layToSphereDotLayDirectionResult;
		// lay�� �� �������� �ּ� �Ÿ��� ���������� ���� ���. ũ�� ���� ���� ���� ���� 2�������� �ɷ���
		if (layAndSphereLeastLengthSquare <= object->sphereCollider.radius * object->sphereCollider.radius)
		{
			// ������� �ϴ� �浹�� �ֱ� �ϴϱ� �浹 ��ġ�� ���ϴ°���
			layDirection = Vector3::Normalize(layDirection);// Ȥ�� �𸣴ϱ�..
			float t = sqrt(Vector3::SquareMagnitude(layToSphere) - layAndSphereLeastLengthSquare);
			float halfContactPointsGap = sqrt(object->sphereCollider.radius * object->sphereCollider.radius - layAndSphereLeastLengthSquare);

			// ���� ���� 1���� ���
			if (halfContactPointsGap == 0)
			{
				Vector3 contactPoint = layPosition + (layDirection * (t));
			}
			// ���� ���� 2���� ���
			else
			{
				Vector3 firstContactPoint = layPosition + (layDirection * (t - halfContactPointsGap));
				Vector3 SecondContactPoint = layPosition + (layDirection * (t + halfContactPointsGap));
			}
		}
	}
}
void Collision::SphereAndSphere(Object* object1, Object* object2, vector<Contact>& contacts)
{
	Vector3 centerDiff = object1->transform.GetPosition() - object2->transform.GetPosition();
	float centerDistance = Vector3::Magnitude(centerDiff);
	if (centerDistance < object1->sphereCollider.radius + object2->sphereCollider.radius)
	{
		Contact contact;
		contact.object1 = object1;
		contact.object2 = object2;
		contact.point = object2->transform.GetPosition() + (centerDiff / 2.0f);
		contact.normal = Vector3::Normalize(centerDiff);
		contact.penetration = object1->sphereCollider.radius + object2->sphereCollider.radius - centerDistance;
		contacts.push_back(contact);
	}
}
void Collision::SphereAndPlaneSpace(Object* object1, Object* object2, vector<Contact>& contacts)
{
	float distance = Vector3::Dot(object2->planeCollider.normal, object1->transform.GetPosition()) - object2->planeCollider.offset - object1->sphereCollider.radius;
	// ���� ����� �Ÿ� ���� + ���� �ƴ϶� ���ϱ� radius�� ����Ѱ���
	if (distance < 0)
	{
		Contact contact;
		contact.object1 = object1;
		contact.object2 = object2;
		contact.point = object1->transform.GetPosition() - object2->planeCollider.normal * (object1->sphereCollider.radius + distance);
		contact.normal = object2->planeCollider.normal;
		contact.penetration = distance;
		contacts.push_back(contact);
	}
}
void Collision::SphereAndPlane(Object* object1, Object* object2, vector<Contact>& contacts)
{
	float centerDistance = Vector3::Dot(object2->planeCollider.normal, object1->transform.GetPosition()) - object2->planeCollider.offset;
	// ���� ����� �Ÿ� ���� + ���� �ƴ϶� ���ϱ� radius�� ����Ѱ���
	if (centerDistance < object1->sphereCollider.radius && centerDistance > -object1->sphereCollider.radius)
	{
		Contact contact;
		contact.object1 = object1;
		contact.object2 = object2;
		contact.point = object1->transform.GetPosition() - object2->planeCollider.normal * centerDistance;
		contact.normal = object2->planeCollider.normal;
		contact.penetration = centerDistance - object1->sphereCollider.radius;
		// ����� ����� �� �����ϱ� ��� �� �n
		if (centerDistance < 0)
		{
			contact.normal *= -1;
			contact.penetration *= -1;
		}
		contacts.push_back(contact);
	}
}
void Collision::SphereAndCube(Object* object1, Object* object2, vector<Contact>& contacts)
{
	// ť���� x, y, z ���� �ȿ� ���� �������� ������ Ȯ���ϱ� ���� ť�� �������� ��ǥ�� �ٲ���
	Vector3 cubeCoordSphereCenterPosition = object2->transform.GetWorldMatrix().Inverse() * object1->transform.GetPosition();
	Vector3 cubeHalfScale = object2->transform.GetScale() / 2;
	// ť���� x, y, z ���� �ȿ� ���� �������� Ȯ��(���͵� ���������� ���ݾ� ��ģ ���·� �浹�� �ƴ� �� �־ �� �� Ȯ���ؾ��ϱ� ��)
	if (abs(cubeCoordSphereCenterPosition.x) - object1->sphereCollider.radius < cubeHalfScale.x &&
		abs(cubeCoordSphereCenterPosition.y) - object1->sphereCollider.radius < cubeHalfScale.y &&
		abs(cubeCoordSphereCenterPosition.z) - object1->sphereCollider.radius < cubeHalfScale.z)
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
		if (Vector3::Magnitude(cubeCoordSphereCenterPosition - closestPoint) <= object1->sphereCollider.radius)
		{
			Vector3 worldCoordClosestPoint = object2->transform.GetWorldMatrix() * closestPoint;
			Contact contact;
			contact.object1= object1;
			contact.object2 = object2;
			contact.point = worldCoordClosestPoint;
			// �̰� ������ �Ѿ �� ���Ĺ����� ����� �ƿ� �ݴ�� �ɵ�, �����ع����� �ذ��ϴ°ſ� �����ؼ� �ذ��ؾ��ϴ� ����?
			contact.normal = Vector3::Normalize(object1->transform.GetPosition() - worldCoordClosestPoint);
			contact.penetration = object1->sphereCollider.radius - Vector3::Magnitude(object1->transform.GetPosition() - worldCoordClosestPoint);
			contacts.push_back(contact);
		}
	}
	// 287pg ������ ��
}
void Collision::CubeAndPlaneSpace(Object* object1, Object* object2, vector<Contact>& contacts)
{
	Vector3 cubeHalfScale = object1->transform.GetScale() / 2;
	// ť���� �� ���� ����� �浹�� Ȯ����
	Vector3 vertexPositions[8] = {
		object1->transform.GetWorldMatrix() * Vector3(cubeHalfScale.x, cubeHalfScale.y, cubeHalfScale.z),
		object1->transform.GetWorldMatrix() * Vector3(-cubeHalfScale.x, cubeHalfScale.y, cubeHalfScale.z),
		object1->transform.GetWorldMatrix() * Vector3(-cubeHalfScale.x, cubeHalfScale.y, -cubeHalfScale.z),
		object1->transform.GetWorldMatrix() * Vector3(cubeHalfScale.x, cubeHalfScale.y, -cubeHalfScale.z),
		object1->transform.GetWorldMatrix() * Vector3(cubeHalfScale.x, -cubeHalfScale.y, cubeHalfScale.z),
		object1->transform.GetWorldMatrix() * Vector3(-cubeHalfScale.x, -cubeHalfScale.y, cubeHalfScale.z),
		object1->transform.GetWorldMatrix() * Vector3(-cubeHalfScale.x, -cubeHalfScale.y, -cubeHalfScale.z),
		object1->transform.GetWorldMatrix() * Vector3(cubeHalfScale.x, -cubeHalfScale.y, -cubeHalfScale.z)
	};
	for (int i = 0; i < 8; i++)
	{
		// ���� ������ ���� ���(��, �������� 0) �浹 Ȯ���̶� ����
		float distance = Vector3::Dot(object2->planeCollider.normal, vertexPositions[i]) - object2->planeCollider.offset;
		if (distance < 0)
		{
			Contact contact;
			contact.object1 = object1;
			contact.object2 = object2;
			contact.point = vertexPositions[i] - object2->planeCollider.normal * distance;
			contact.normal = object2->planeCollider.normal;
			contact.penetration = distance;
			contacts.push_back(contact);
		}
	}
}
void Collision::CubeAndCube(Object* object1, Object* object2, vector<Contact>& contacts)
{
	float cube1DotXAxisResult = Vector3::Dot(object1->transform.GetRotationMatrix() * object1->transform.GetScalingMatrix() * Vector3::One(), Vector3::Right());
	float cube2DotXAxisResult = Vector3::Dot(object2->transform.GetRotationMatrix() * object2->transform.GetScalingMatrix() * Vector3::One(), Vector3::Right());
	float cubeCenterDiffDotXAxisResult = Vector3::Dot(object1->transform.GetPosition() - object2->transform.GetPosition(), Vector3::Right());
	float cube1DotYAxisResult = Vector3::Dot(object1->transform.GetRotationMatrix() * object1->transform.GetScalingMatrix() * Vector3::One(), Vector3::Up());
	float cube2DotYAxisResult = Vector3::Dot(object2->transform.GetRotationMatrix() * object2->transform.GetScalingMatrix() * Vector3::One(), Vector3::Up());
	float cubeCenterDiffDotYAxisResult = Vector3::Dot(object1->transform.GetPosition() - object2->transform.GetPosition(), Vector3::Up());
	float cube1DotZAxisResult = Vector3::Dot(object1->transform.GetRotationMatrix() * object1->transform.GetScalingMatrix() * Vector3::One(), Vector3::Forward());
	float cube2DotZAxisResult = Vector3::Dot(object2->transform.GetRotationMatrix() * object2->transform.GetScalingMatrix() * Vector3::One(), Vector3::Forward());
	float cubeCenterDiffDotZAxisResult = Vector3::Dot(object1->transform.GetPosition() - object2->transform.GetPosition(), Vector3::Forward());

	// �����ϰ� üũ�غ��� �ϴ� ���� �� 15���� ������ ����
	// Cube1�� x, y, z �� 3��
	// Cube2�� x, y, z �� 3��
	// Cbue1�� x, y, z ��� Cube2�� x, y, z ���� ���� ���� �� �������� ���� Cross(xx), Cross(xy), Cross(xz), Cross(yx), Cross(yy), Cross(yz), Cross(zx), Cross(zy), Cross(zz) �� 9��
	Vector3 axis;
	// 1. Cube1�� x, y, z �� 3��
	Vector3 cube1CoordCube2CenterPosition = object1->transform.GetWorldMatrix().Inverse() * object2->transform.GetPosition();
	Vector3 cube1CoordCube2Scale = object1->transform.GetWorldMatrix().Inverse() * object2->transform.GetPosition();
	//Vector3::Dot()


	// x, y, z�࿡ ��� �����ؼ� ��ġ���� Ȯ��
	if (cube1DotXAxisResult + cube2DotXAxisResult <= 2 * cubeCenterDiffDotXAxisResult &&
		cube1DotYAxisResult + cube2DotYAxisResult <= 2 * cubeCenterDiffDotYAxisResult &&
		cube1DotZAxisResult + cube2DotZAxisResult <= 2 * cubeCenterDiffDotZAxisResult)
	{

	}
}