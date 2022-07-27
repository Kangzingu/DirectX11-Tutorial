#include "Object.h"
#include "Collision.h"
/************************************/
/* 물체-1 이 물체-2 에 가서 박는것! */
/************************************/

void Collision::LayAndSphere(Vector3& layPosition, Vector3& layDirection, Object* object)
{
	Vector3 layToSphere = object->transform.GetPosition() - layPosition;
	// lay(카메라)의 원점이 구 안에 있는 경우. 상황에 따라 알아서 처리하도록..
	if (Vector3::SquareMagnitude(layToSphere) < object->sphereCollider.radius * object->sphereCollider.radius)
	{
	}
	float layToSphereDotLayDirectionResult = Vector3::Dot(layToSphere, layDirection);
	// lay(카메라)의 진행방향에 구가 있는 경우. 후면에 구가 있으면 1차적으로 걸러짐
	if (layToSphereDotLayDirectionResult >= 0)
	{
		float layAndSphereLeastLengthSquare = Vector3::SquareMagnitude(layToSphere) - layToSphereDotLayDirectionResult * layToSphereDotLayDirectionResult;
		// lay와 구 원점간의 최소 거리가 반지름보다 작은 경우. 크면 둘이 닿을 수가 없어 2차적으로 걸러짐
		if (layAndSphereLeastLengthSquare <= object->sphereCollider.radius * object->sphereCollider.radius)
		{
			// 여기부턴 일단 충돌이 있긴 하니까 충돌 위치를 구하는거임
			layDirection = Vector3::Normalize(layDirection);// 혹시 모르니까..
			float t = sqrt(Vector3::SquareMagnitude(layToSphere) - layAndSphereLeastLengthSquare);
			float halfContactPointsGap = sqrt(object->sphereCollider.radius * object->sphereCollider.radius - layAndSphereLeastLengthSquare);

			// 닿은 점이 1개인 경우
			if (halfContactPointsGap == 0)
			{
				Vector3 contactPoint = layPosition + (layDirection * (t));
			}
			// 닿은 점이 2개인 경우
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
	// 점과 평면의 거리 공식 + 점이 아니라 구니까 radius도 고려한거임
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
	// 점과 평면의 거리 공식 + 점이 아니라 구니까 radius도 고려한거임
	if (centerDistance < object1->sphereCollider.radius && centerDistance > -object1->sphereCollider.radius)
	{
		Contact contact;
		contact.object1 = object1;
		contact.object2 = object2;
		contact.point = object1->transform.GetPosition() - object2->planeCollider.normal * centerDistance;
		contact.normal = object2->planeCollider.normal;
		contact.penetration = centerDistance - object1->sphereCollider.radius;
		// 평면이 양면일 수 있으니까 양면 다 쳌
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
	// 큐브의 x, y, z 범위 안에 구가 들어오는지 간단한 확인하기 위해 큐브 기준으로 좌표를 바꿔줌
	Vector3 cubeCoordSphereCenterPosition = object2->transform.GetWorldMatrix().Inverse() * object1->transform.GetPosition();
	Vector3 cubeHalfScale = object2->transform.GetScale() / 2;
	// 큐브의 x, y, z 범위 안에 구가 들어오는지 확인(들어와도 반지름덕에 조금씩 걸친 상태로 충돌이 아닐 수 있어서 함 더 확인해야하긴 함)
	if (abs(cubeCoordSphereCenterPosition.x) - object1->sphereCollider.radius < cubeHalfScale.x &&
		abs(cubeCoordSphereCenterPosition.y) - object1->sphereCollider.radius < cubeHalfScale.y &&
		abs(cubeCoordSphereCenterPosition.z) - object1->sphereCollider.radius < cubeHalfScale.z)
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
		if (Vector3::Magnitude(cubeCoordSphereCenterPosition - closestPoint) <= object1->sphereCollider.radius)
		{
			Vector3 worldCoordClosestPoint = object2->transform.GetWorldMatrix() * closestPoint;
			Contact contact;
			contact.object1= object1;
			contact.object2 = object2;
			contact.point = worldCoordClosestPoint;
			// 이거 중점을 넘어서 더 겹쳐버리면 계산이 아예 반대로 될듯, 관통해버린걸 해결하는거에 포함해서 해결해야하는 느낌?
			contact.normal = Vector3::Normalize(object1->transform.GetPosition() - worldCoordClosestPoint);
			contact.penetration = object1->sphereCollider.radius - Vector3::Magnitude(object1->transform.GetPosition() - worldCoordClosestPoint);
			contacts.push_back(contact);
		}
	}
	// 287pg 읽으면 댐
}
void Collision::CubeAndPlaneSpace(Object* object1, Object* object2, vector<Contact>& contacts)
{
	Vector3 cubeHalfScale = object1->transform.GetScale() / 2;
	// 큐브의 각 점과 평면의 충돌을 확인함
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
		// 다음 과정은 구와 평면(단, 반지름이 0) 충돌 확인이랑 같음
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

	// 투영하고 체크해봐야 하는 축은 총 15개로 다음과 같음
	// Cube1의 x, y, z 축 3개
	// Cube2의 x, y, z 축 3개
	// Cbue1의 x, y, z 축과 Cube2의 x, y, z 축을 각각 조합 후 외적으로 만든 Cross(xx), Cross(xy), Cross(xz), Cross(yx), Cross(yy), Cross(yz), Cross(zx), Cross(zy), Cross(zz) 축 9개
	Vector3 axis;
	// 1. Cube1의 x, y, z 축 3개
	Vector3 cube1CoordCube2CenterPosition = object1->transform.GetWorldMatrix().Inverse() * object2->transform.GetPosition();
	Vector3 cube1CoordCube2Scale = object1->transform.GetWorldMatrix().Inverse() * object2->transform.GetPosition();
	//Vector3::Dot()


	// x, y, z축에 모두 투영해서 겹치는지 확인
	if (cube1DotXAxisResult + cube2DotXAxisResult <= 2 * cubeCenterDiffDotXAxisResult &&
		cube1DotYAxisResult + cube2DotYAxisResult <= 2 * cubeCenterDiffDotYAxisResult &&
		cube1DotZAxisResult + cube2DotZAxisResult <= 2 * cubeCenterDiffDotZAxisResult)
	{

	}
}