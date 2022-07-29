#include "Object.h"
#include "Collision.h"
/************************************/
/* ��ü-1 �� ��ü-2 �� ���� �ڴ°�! */
/************************************/

void Collision::LayAndSphere(Vector3& layPosition, Vector3& layDirection, Object* object)
{
	Vector3 layToSphere = object->transform.GetPosition() - layPosition;
	// lay(ī�޶�)�� ������ �� �ȿ� �ִ� ���. ��Ȳ�� ���� �˾Ƽ� ó���ϵ���..
	if (Vector3::SquareMagnitude(layToSphere) < object->sphereCollider.GetRadius() * object->sphereCollider.GetRadius())
	{
	}
	float layToSphereDotLayDirectionResult = Vector3::Dot(layToSphere, layDirection);
	// lay(ī�޶�)�� ������⿡ ���� �ִ� ���. �ĸ鿡 ���� ������ 1�������� �ɷ���
	if (layToSphereDotLayDirectionResult >= 0)
	{
		float layAndSphereLeastLengthSquare = Vector3::SquareMagnitude(layToSphere) - layToSphereDotLayDirectionResult * layToSphereDotLayDirectionResult;
		// lay�� �� �������� �ּ� �Ÿ��� ���������� ���� ���. ũ�� ���� ���� ���� ���� 2�������� �ɷ���
		if (layAndSphereLeastLengthSquare <= object->sphereCollider.GetRadius() * object->sphereCollider.GetRadius())
		{
			// ������� �ϴ� �浹�� �ֱ� �ϴϱ� �浹 ��ġ�� ���ϴ°���
			layDirection = Vector3::Normalize(layDirection);// Ȥ�� �𸣴ϱ�..
			float t = sqrt(Vector3::SquareMagnitude(layToSphere) - layAndSphereLeastLengthSquare);
			float halfContactPointsGap = sqrt(object->sphereCollider.GetRadius() * object->sphereCollider.GetRadius() - layAndSphereLeastLengthSquare);

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
	// �̰� square magnitude �� �ٲ㼭 ��Ʈ ����� ���� ������ ���߿�
	float centerDistance = Vector3::Magnitude(centerDiff);
	if (centerDistance < object1->sphereCollider.GetRadius() + object2->sphereCollider.GetRadius())
	{
		Contact contact;
		contact.object1 = object1;
		contact.object2 = object2;
		contact.point = object2->transform.GetPosition() + (centerDiff / 2.0f);
		contact.normal = Vector3::Normalize(centerDiff);
		contact.penetration = object1->sphereCollider.GetRadius() + object2->sphereCollider.GetRadius() - centerDistance;
		contacts.push_back(contact);
	}
}
void Collision::SphereAndPlaneSpace(Object* object1, Object* object2, vector<Contact>& contacts)
{
	float distance = Vector3::Dot(object2->planeCollider.GetNormal(), object1->transform.GetPosition()) - object2->planeCollider.GetOffset() - object1->sphereCollider.GetRadius();
	// ���� ����� �Ÿ� ���� + ���� �ƴ϶� ���ϱ� radius�� ����Ѱ���
	if (distance < 0)
	{
		Contact contact;
		contact.object1 = object1;
		contact.object2 = object2;
		contact.point = object1->transform.GetPosition() - object2->planeCollider.GetNormal() * (object1->sphereCollider.GetRadius()+ distance);
		contact.normal = object2->planeCollider.GetNormal();
		contact.penetration = distance;
		contacts.push_back(contact);
	}
}
void Collision::SphereAndPlane(Object* object1, Object* object2, vector<Contact>& contacts)
{
	float centerDistance = Vector3::Dot(object2->planeCollider.GetNormal(), object1->transform.GetPosition()) - object2->planeCollider.GetOffset();
	// ���� ����� �Ÿ� ���� + ���� �ƴ϶� ���ϱ� radius�� ����Ѱ���
	if (centerDistance < object1->sphereCollider.GetRadius() && centerDistance > -object1->sphereCollider.GetRadius())
	{
		Contact contact;
		contact.object1 = object1;
		contact.object2 = object2;
		contact.point = object1->transform.GetPosition() - object2->planeCollider.GetNormal() * centerDistance;
		contact.normal = object2->planeCollider.GetNormal();
		contact.penetration = centerDistance - object1->sphereCollider.GetRadius();
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
	Vector3 cubeHalfScale = object2->transform.GetScale() / 2.0f;
	// ť���� x, y, z ���� �ȿ� ���� �������� Ȯ��(���͵� ���������� ���ݾ� ��ģ ���·� �浹�� �ƴ� �� �־ �� �� Ȯ���ؾ��ϱ� ��)
	if (abs(cubeCoordSphereCenterPosition.x) - object1->sphereCollider.GetRadius() < cubeHalfScale.x &&
		abs(cubeCoordSphereCenterPosition.y) - object1->sphereCollider.GetRadius() < cubeHalfScale.y &&
		abs(cubeCoordSphereCenterPosition.z) - object1->sphereCollider.GetRadius() < cubeHalfScale.z)
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
		if (Vector3::Magnitude(cubeCoordSphereCenterPosition - closestPoint) <= object1->sphereCollider.GetRadius())
		{
			Vector3 worldCoordClosestPoint = object2->transform.GetWorldMatrix() * closestPoint;
			Contact contact;
			contact.object1= object1;
			contact.object2 = object2;
			contact.point = worldCoordClosestPoint;
			// �̰� ������ �Ѿ �� ���Ĺ����� ����� �ƿ� �ݴ�� �ɵ�, �����ع����� �ذ��ϴ°ſ� �����ؼ� �ذ��ؾ��ϴ� ����?
			contact.normal = Vector3::Normalize(object1->transform.GetPosition() - worldCoordClosestPoint);
			contact.penetration = object1->sphereCollider.GetRadius() - Vector3::Magnitude(object1->transform.GetPosition() - worldCoordClosestPoint);
			contacts.push_back(contact);
		}
	}
	// 287pg ������ ��
}
void Collision::CubeAndPlaneSpace(Object* object1, Object* object2, vector<Contact>& contacts)
{
	Vector3 cubeHalfScale = object1->transform.GetScale() / 2.0f;
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
		float distance = Vector3::Dot(object2->planeCollider.GetNormal(), vertexPositions[i]) - object2->planeCollider.GetOffset();
		if (distance < 0)
		{
			Contact contact;
			contact.object1 = object1;
			contact.object2 = object2;
			contact.point = vertexPositions[i] - object2->planeCollider.GetNormal() * distance;
			contact.normal = object2->planeCollider.GetNormal();
			contact.penetration = distance;
			contacts.push_back(contact);
		}
	}
}
// ť�� �� �ܼ� �浹���� Ȯ�ο� ���
float ProjectObjectToAxis(Object& object, Vector3 axis)
{
	return abs(Vector3::Dot(object.transform.GetRight() * object.transform.GetScale().x, axis)) +
		abs(Vector3::Dot(object.transform.GetUp() * object.transform.GetScale().y, axis)) +
		abs(Vector3::Dot(object.transform.GetForward() * object.transform.GetScale().z, axis));
}
// ť�� �� �� �浹 ������ Ȯ�ο� ���
bool CubeAndPoint(Object& object1, Object& object2, Vector3 point, vector<Contact>& contacts)
{
	Vector3 cubeCoordPoint = object2.transform.GetRotationMatrix().Inverse() * object2.transform.GetTranslationMatrix().Inverse() * point;
	Vector3 cubeHalfScale = object2.transform.GetScale() / 2.0f;
	Vector3 normal;
	float penetration;
	float minPenetration;

	penetration = cubeHalfScale.x - abs(cubeCoordPoint.x);
	// ������ �ʴ´ٸ�
	if (penetration < 0)
		return false;
	// �����ٸ�
	minPenetration = penetration;
	if (cubeCoordPoint.x >= 0)
		normal = object2.transform.GetRight();
	else
		normal = -object2.transform.GetRight();

	penetration = cubeHalfScale.y - abs(cubeCoordPoint.y);
	// ������ �ʴ´ٸ�
	if (penetration < 0)
		return false;
	// �����ٸ�
	if (minPenetration > penetration)
	{
		minPenetration = penetration;
		if (cubeCoordPoint.y >= 0)
			normal = object2.transform.GetUp();
		else
			normal = -object2.transform.GetUp();
	}

	penetration = cubeHalfScale.z - abs(cubeCoordPoint.z);
	// ������ �ʴ´ٸ�
	if (penetration < 0)
		return false;
	// �����ٸ�
	if (minPenetration > penetration)
	{
		minPenetration = penetration;
		if (cubeCoordPoint.z >= 0)
			normal = object2.transform.GetForward();
		else
			normal = -object2.transform.GetForward();
	}

	// �̰� ��¦ �̻���.. �ݴ��� ��ü�� ���ٴ°�..
	// 20220728_�ϴ� �ݴ��� ��ü �ִ°ɷ� ��� ���ļ� ������.. ���� �ڵ� �̰���
	// contact.object1 = nullptr;
	// contact.object2 = &object1;
	
	Contact contact;
	contact.object1 = &object1;
	contact.object2 = &object2;
	contact.point = point;
	contact.normal = normal;
	contact.penetration = minPenetration;
	contacts.push_back(contact);
}
// ť�� �� �� �浹 ������ Ȯ�ο� ���
void CubeAndEdge(Object& object1, Object& object2, LineSegment edge, vector<Contact>& contacts)
{
	//    2 ------- 3
	//   /|        /|
	//  1 ------- 0 7
	// 	|/        |/
	// 	5 ------- 4
	// �ð�������� �� 12�� ����
	//	0	1
	//	1	2
	// 	2	3
	//	3	0
	//	0	4
	//	1	5
	//	2	6
	//	3	7
	//	4	5
	//	5	6
	//	6	7
	//	7	4
	vector<LineSegment> cubeEdges;
	Vector3 closestPointOnCubeEdge;
	Vector3 closestPointOnEdge;
	float penetration;

	Vector3 contactPointOnCubeEdge;
	Vector3 contactPointOnEdge;
	float minPenetration = 1000;

	cubeEdges.push_back(LineSegment(object2.transform.GetPosition() + (object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y + object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetRight(), object2.transform.GetScale().x));
	cubeEdges.push_back(LineSegment(object2.transform.GetPosition() + (-object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y + object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetForward(), object2.transform.GetScale().z));
	cubeEdges.push_back(LineSegment(object2.transform.GetPosition() + (-object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y - object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, object2.transform.GetRight(), object2.transform.GetScale().x));
	cubeEdges.push_back(LineSegment(object2.transform.GetPosition() + (object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y - object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, object2.transform.GetForward(), object2.transform.GetScale().z));
	cubeEdges.push_back(LineSegment(object2.transform.GetPosition() + (object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y + object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetUp(), object2.transform.GetScale().y));
	cubeEdges.push_back(LineSegment(object2.transform.GetPosition() + (-object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y + object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetUp(), object2.transform.GetScale().y));
	cubeEdges.push_back(LineSegment(object2.transform.GetPosition() + (-object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y - object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetUp(), object2.transform.GetScale().y));
	cubeEdges.push_back(LineSegment(object2.transform.GetPosition() + (object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y - object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetUp(), object2.transform.GetScale().y));
	cubeEdges.push_back(LineSegment(object2.transform.GetPosition() + (object2.transform.GetRight() * object2.transform.GetScale().x - object2.transform.GetUp() * object2.transform.GetScale().y + object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetRight(), object2.transform.GetScale().x));
	cubeEdges.push_back(LineSegment(object2.transform.GetPosition() + (-object2.transform.GetRight() * object2.transform.GetScale().x - object2.transform.GetUp() * object2.transform.GetScale().y + object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetForward(), object2.transform.GetScale().z));
	cubeEdges.push_back(LineSegment(object2.transform.GetPosition() + (-object2.transform.GetRight() * object2.transform.GetScale().x - object2.transform.GetUp() * object2.transform.GetScale().y - object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, object2.transform.GetRight(), object2.transform.GetScale().x));
	cubeEdges.push_back(LineSegment(object2.transform.GetPosition() + (object2.transform.GetRight() * object2.transform.GetScale().x - object2.transform.GetUp() * object2.transform.GetScale().y - object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, object2.transform.GetForward(), object2.transform.GetScale().z));
	for (int i = 0; i < cubeEdges.size(); i++)
	{
		// ť���� �� ���а� ���Ϸ��� ���� �� �ִܰŸ� ���(Real Time Rendering 2 675pg)
		// Line Segment1 = o1 + s * d1
		// Line Segment2 = o2 + t + d2
		Vector3 o1 = cubeEdges[i].origin;
		Vector3 o2 = edge.origin;
		Vector3 d1 = cubeEdges[i].direction;
		Vector3 d2 = edge.direction;
		float s, t;
		if (Vector3::Cross(d1, d2) == Vector3::Zero())
		{
			// �� ��� �� ������ ������ ��
			continue;
		}
		s = Vector3::Dot(Vector3::Cross((o2 - o1), d2), Vector3::Cross(d1, d2)) / Vector3::Dot(Vector3::Cross(d1, d2), Vector3::Cross(d1, d2));
		t = Vector3::Dot(Vector3::Cross((o1 - o2), d1), Vector3::Cross(d2, d1)) / Vector3::Dot(Vector3::Cross(d2, d1), Vector3::Cross(d2, d1));
		if (s<0 || s>cubeEdges[i].length ||
			t<0 || t>edge.length)
		{
			continue;
			// �� ��� �������� ����
		}
		closestPointOnCubeEdge = cubeEdges[i].origin + cubeEdges[i].direction * s;
		closestPointOnEdge = edge.origin + edge.direction * t;

		// �̰� �ڽ��� �Ѿ��� ������(����� �� ���ʿ� ġ��ģ��)�� �ϸ� ����� �� �̻�����.. �ٸ� ��� ������
		// ¶�� �̰� ���ľ��� closestPointOnCubeEdge - closestPointOnEdge �� �ƴϰ� object2.transform.GetPosition() - closestPointOnEdge �̷������� �ϴ���
		// �̷����̶�� �ᱹ �� �浹�˻� ��ĺ��� �켱������ ������ ��. �ҾȺҾ��� �κ��� �����Ű���

		penetration = Vector3::Magnitude(closestPointOnCubeEdge - closestPointOnEdge);
		// �� ��� ť�� �������� ť���� �������� �ٸ� ������ �� �����Ƿ� �浹�� ����
		if (Vector3::SquareMagnitude(object2.transform.GetPosition() - closestPointOnCubeEdge) > Vector3::SquareMagnitude(object2.transform.GetPosition() - closestPointOnEdge))
		{
			// �׷��ٸ� ��ǻ� ť���� ��� ������ �� �浹�Ȱɷ� ���ðŶ� ���� ��� �浹�� ���� ��󳾰���
			// 
			if (penetration < minPenetration)
			{
				minPenetration = penetration;
				contactPointOnCubeEdge = closestPointOnCubeEdge;
				contactPointOnEdge = closestPointOnEdge;
			}
		}
	}
	if (contactPointOnCubeEdge - contactPointOnEdge == Vector3::Zero())
		return;
	// �̰� ��¦ �̻���.. �ݴ��� ��ü�� ���ٴ°�..
	Contact contact;
	contact.object1 = &object1;
	contact.object2 = &object2;
	contact.point = contactPointOnEdge;
	contact.normal = Vector3::Normalize(contactPointOnCubeEdge - contactPointOnEdge);
	contact.penetration = minPenetration;
	contacts.push_back(contact);
}
bool Collision::CubeAndCube(Object* object1, Object* object2, vector<Contact>& contacts)
{
	// �����ϰ� üũ�غ��� �ϴ� ���� �� 15���� ������ ����
	// Cube1�� x, y, z �� 3��
	// Cube2�� x, y, z �� 3��
	// Cbue1�� x, y, z ��� Cube2�� x, y, z ���� ���� ���� �� �������� ���� Cross(xx), Cross(xy), Cross(xz), Cross(yx), Cross(yy), Cross(yz), Cross(zx), Cross(zy), Cross(zz) �� 9��	
	vector<Vector3> axes;
	axes.push_back(object1->transform.GetRight());
	axes.push_back(object1->transform.GetUp());
	axes.push_back(object1->transform.GetForward());
	axes.push_back(object2->transform.GetRight());
	axes.push_back(object2->transform.GetUp());
	axes.push_back(object2->transform.GetForward());
	if (Vector3::Cross(object1->transform.GetRight(), object2->transform.GetRight()) != Vector3::Zero())
		axes.push_back(Vector3::Normalize(Vector3::Cross(object1->transform.GetRight(), object2->transform.GetRight())));
	if (Vector3::Cross(object1->transform.GetRight(), object2->transform.GetUp()) != Vector3::Zero())
		axes.push_back(Vector3::Normalize(Vector3::Cross(object1->transform.GetRight(), object2->transform.GetUp())));
	if (Vector3::Cross(object1->transform.GetRight(), object2->transform.GetForward()) != Vector3::Zero())
		axes.push_back(Vector3::Normalize(Vector3::Cross(object1->transform.GetRight(), object2->transform.GetForward())));
	if (Vector3::Cross(object1->transform.GetUp(), object2->transform.GetRight()) != Vector3::Zero())
		axes.push_back(Vector3::Normalize(Vector3::Cross(object1->transform.GetUp(), object2->transform.GetRight())));
	if (Vector3::Cross(object1->transform.GetUp(), object2->transform.GetUp()) != Vector3::Zero())
		axes.push_back(Vector3::Normalize(Vector3::Cross(object1->transform.GetUp(), object2->transform.GetUp())));
	if (Vector3::Cross(object1->transform.GetUp(), object2->transform.GetForward()) != Vector3::Zero())
		axes.push_back(Vector3::Normalize(Vector3::Cross(object1->transform.GetUp(), object2->transform.GetForward())));
	if (Vector3::Cross(object1->transform.GetForward(), object2->transform.GetRight()) != Vector3::Zero())
		axes.push_back(Vector3::Normalize(Vector3::Cross(object1->transform.GetForward(), object2->transform.GetRight())));
	if (Vector3::Cross(object1->transform.GetForward(), object2->transform.GetUp()) != Vector3::Zero())
		axes.push_back(Vector3::Normalize(Vector3::Cross(object1->transform.GetForward(), object2->transform.GetUp())));
	if (Vector3::Cross(object1->transform.GetForward(), object2->transform.GetForward()) != Vector3::Zero())
		axes.push_back(Vector3::Normalize(Vector3::Cross(object1->transform.GetForward(), object2->transform.GetForward())));
	for (int i = 0; i < axes.size(); i++)
	{
		float object1LengthOnAxis = ProjectObjectToAxis(*object1, axes[i]);
		float object2LengthOnAxis = ProjectObjectToAxis(*object2, axes[i]);
		float objectCenterDistanceOnAxis = abs(Vector3::Dot(object1->transform.GetPosition() - object2->transform.GetPosition(), axes[i]));
		if (object1LengthOnAxis + object2LengthOnAxis < objectCenterDistanceOnAxis * 2)
		{
			return false;
		}
	}
	// 1�� �˻� ���! ������� �Դٴ°� �����ϰ� �ִٴ� ����(������ Ȯ����), ���� ���ϰ� ��� ��ŭ �浹�ߴ��� ã�°���

	vector<LineSegment> cube1Edges;
	cube1Edges.push_back(LineSegment(object1->transform.GetPosition() + (object1->transform.GetRight() * object1->transform.GetScale().x + object1->transform.GetUp() * object1->transform.GetScale().y + object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f, -object1->transform.GetRight(), object1->transform.GetScale().x));
	cube1Edges.push_back(LineSegment(object1->transform.GetPosition() + (-object1->transform.GetRight() * object1->transform.GetScale().x + object1->transform.GetUp() * object1->transform.GetScale().y + object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f, -object1->transform.GetForward(), object1->transform.GetScale().z));
	cube1Edges.push_back(LineSegment(object1->transform.GetPosition() + (-object1->transform.GetRight() * object1->transform.GetScale().x + object1->transform.GetUp() * object1->transform.GetScale().y - object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f, object1->transform.GetRight(), object1->transform.GetScale().x));
	cube1Edges.push_back(LineSegment(object1->transform.GetPosition() + (object1->transform.GetRight() * object1->transform.GetScale().x + object1->transform.GetUp() * object1->transform.GetScale().y - object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f, object1->transform.GetForward(), object1->transform.GetScale().z));
	cube1Edges.push_back(LineSegment(object1->transform.GetPosition() + (object1->transform.GetRight() * object1->transform.GetScale().x + object1->transform.GetUp() * object1->transform.GetScale().y + object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f, -object1->transform.GetUp(), object1->transform.GetScale().y));
	cube1Edges.push_back(LineSegment(object1->transform.GetPosition() + (-object1->transform.GetRight() * object1->transform.GetScale().x + object1->transform.GetUp() * object1->transform.GetScale().y + object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f, -object1->transform.GetUp(), object1->transform.GetScale().y));
	cube1Edges.push_back(LineSegment(object1->transform.GetPosition() + (-object1->transform.GetRight() * object1->transform.GetScale().x + object1->transform.GetUp() * object1->transform.GetScale().y - object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f, -object1->transform.GetUp(), object1->transform.GetScale().y));
	cube1Edges.push_back(LineSegment(object1->transform.GetPosition() + (object1->transform.GetRight() * object1->transform.GetScale().x + object1->transform.GetUp() * object1->transform.GetScale().y - object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f, -object1->transform.GetUp(), object1->transform.GetScale().y));
	cube1Edges.push_back(LineSegment(object1->transform.GetPosition() + (object1->transform.GetRight() * object1->transform.GetScale().x - object1->transform.GetUp() * object1->transform.GetScale().y + object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f, -object1->transform.GetRight(), object1->transform.GetScale().x));
	cube1Edges.push_back(LineSegment(object1->transform.GetPosition() + (-object1->transform.GetRight() * object1->transform.GetScale().x - object1->transform.GetUp() * object1->transform.GetScale().y + object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f, -object1->transform.GetForward(), object1->transform.GetScale().z));
	cube1Edges.push_back(LineSegment(object1->transform.GetPosition() + (-object1->transform.GetRight() * object1->transform.GetScale().x - object1->transform.GetUp() * object1->transform.GetScale().y - object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f, object1->transform.GetRight(), object1->transform.GetScale().x));
	cube1Edges.push_back(LineSegment(object1->transform.GetPosition() + (object1->transform.GetRight() * object1->transform.GetScale().x - object1->transform.GetUp() * object1->transform.GetScale().y - object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f, object1->transform.GetForward(), object1->transform.GetScale().z));
	for (int i = 0; i < cube1Edges.size(); i++)
	{
		CubeAndEdge(*object1, *object2, cube1Edges[i], contacts);
	}
	vector<LineSegment> cube2Edges;
	cube2Edges.push_back(LineSegment(object2->transform.GetPosition() + (object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y + object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f, -object2->transform.GetRight(), object2->transform.GetScale().x));
	cube2Edges.push_back(LineSegment(object2->transform.GetPosition() + (-object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y + object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f, -object2->transform.GetForward(), object2->transform.GetScale().z));
	cube2Edges.push_back(LineSegment(object2->transform.GetPosition() + (-object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y - object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f, object2->transform.GetRight(), object2->transform.GetScale().x));
	cube2Edges.push_back(LineSegment(object2->transform.GetPosition() + (object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y - object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f, object2->transform.GetForward(), object2->transform.GetScale().z));
	cube2Edges.push_back(LineSegment(object2->transform.GetPosition() + (object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y + object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f, -object2->transform.GetUp(), object2->transform.GetScale().y));
	cube2Edges.push_back(LineSegment(object2->transform.GetPosition() + (-object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y + object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f, -object2->transform.GetUp(), object2->transform.GetScale().y));
	cube2Edges.push_back(LineSegment(object2->transform.GetPosition() + (-object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y - object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f, -object2->transform.GetUp(), object2->transform.GetScale().y));
	cube2Edges.push_back(LineSegment(object2->transform.GetPosition() + (object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y - object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f, -object2->transform.GetUp(), object2->transform.GetScale().y));
	cube2Edges.push_back(LineSegment(object2->transform.GetPosition() + (object2->transform.GetRight() * object2->transform.GetScale().x - object2->transform.GetUp() * object2->transform.GetScale().y + object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f, -object2->transform.GetRight(), object2->transform.GetScale().x));
	cube2Edges.push_back(LineSegment(object2->transform.GetPosition() + (-object2->transform.GetRight() * object2->transform.GetScale().x - object2->transform.GetUp() * object2->transform.GetScale().y + object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f, -object2->transform.GetForward(), object2->transform.GetScale().z));
	cube2Edges.push_back(LineSegment(object2->transform.GetPosition() + (-object2->transform.GetRight() * object2->transform.GetScale().x - object2->transform.GetUp() * object2->transform.GetScale().y - object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f, object2->transform.GetRight(), object2->transform.GetScale().x));
	cube2Edges.push_back(LineSegment(object2->transform.GetPosition() + (object2->transform.GetRight() * object2->transform.GetScale().x - object2->transform.GetUp() * object2->transform.GetScale().y - object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f, object2->transform.GetForward(), object2->transform.GetScale().z));
	for (int i = 0; i < cube2Edges.size(); i++)
	{
		CubeAndEdge(*object2, *object1, cube2Edges[i], contacts);
	}

	vector<Vector3> cube1Vertices;
	cube1Vertices.push_back(Vector3(object1->transform.GetPosition() + (object1->transform.GetRight() * object1->transform.GetScale().x + object1->transform.GetUp() * object1->transform.GetScale().y + object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f));
	cube1Vertices.push_back(Vector3(object1->transform.GetPosition() + (-object1->transform.GetRight() * object1->transform.GetScale().x + object1->transform.GetUp() * object1->transform.GetScale().y + object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f));
	cube1Vertices.push_back(Vector3(object1->transform.GetPosition() + (-object1->transform.GetRight() * object1->transform.GetScale().x + object1->transform.GetUp() * object1->transform.GetScale().y - object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f));
	cube1Vertices.push_back(Vector3(object1->transform.GetPosition() + (object1->transform.GetRight() * object1->transform.GetScale().x + object1->transform.GetUp() * object1->transform.GetScale().y - object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f));
	cube1Vertices.push_back(Vector3(object1->transform.GetPosition() + (object1->transform.GetRight() * object1->transform.GetScale().x - object1->transform.GetUp() * object1->transform.GetScale().y + object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f));
	cube1Vertices.push_back(Vector3(object1->transform.GetPosition() + (-object1->transform.GetRight() * object1->transform.GetScale().x - object1->transform.GetUp() * object1->transform.GetScale().y + object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f));
	cube1Vertices.push_back(Vector3(object1->transform.GetPosition() + (-object1->transform.GetRight() * object1->transform.GetScale().x - object1->transform.GetUp() * object1->transform.GetScale().y - object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f));
	cube1Vertices.push_back(Vector3(object1->transform.GetPosition() + (object1->transform.GetRight() * object1->transform.GetScale().x - object1->transform.GetUp() * object1->transform.GetScale().y - object1->transform.GetForward() * object1->transform.GetScale().z) / 2.0f));
	for (int i = 0; i < cube1Vertices.size(); i++)
	{
		CubeAndPoint(*object1, *object2, cube1Vertices[i], contacts);
	}
	vector<Vector3> cube2Vertices;
	cube2Vertices.push_back(Vector3(object2->transform.GetPosition() + (object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y + object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f));
	cube2Vertices.push_back(Vector3(object2->transform.GetPosition() + (-object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y + object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f));
	cube2Vertices.push_back(Vector3(object2->transform.GetPosition() + (-object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y - object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f));
	cube2Vertices.push_back(Vector3(object2->transform.GetPosition() + (object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y - object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f));
	cube2Vertices.push_back(Vector3(object2->transform.GetPosition() + (object2->transform.GetRight() * object2->transform.GetScale().x - object2->transform.GetUp() * object2->transform.GetScale().y + object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f));
	cube2Vertices.push_back(Vector3(object2->transform.GetPosition() + (-object2->transform.GetRight() * object2->transform.GetScale().x - object2->transform.GetUp() * object2->transform.GetScale().y + object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f));
	cube2Vertices.push_back(Vector3(object2->transform.GetPosition() + (-object2->transform.GetRight() * object2->transform.GetScale().x - object2->transform.GetUp() * object2->transform.GetScale().y - object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f));
	cube2Vertices.push_back(Vector3(object2->transform.GetPosition() + (object2->transform.GetRight() * object2->transform.GetScale().x - object2->transform.GetUp() * object2->transform.GetScale().y - object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f));
	for (int i = 0; i < cube2Vertices.size(); i++)
	{
		CubeAndPoint(*object2, *object1, cube2Vertices[i], contacts);
	}
	return true;
	// 303pg ������ ��
}