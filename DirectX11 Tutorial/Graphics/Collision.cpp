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
	// �̰� square magnitude �� �ٲ㼭 ��Ʈ ����� ���� ������ ���߿�
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
	Vector3 cubeHalfScale = object2->transform.GetScale() / 2.0f;
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
	Vector3 cubeCoordPoint = object1.transform.GetRotationMatrix().Inverse() * object1.transform.GetTranslationMatrix().Inverse() * point;
	Vector3 cubeHalfScale = object1.transform.GetScale() / 2.0f;
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
		normal = object1.transform.GetRight();
	else
		normal = -object1.transform.GetRight();

	penetration = cubeHalfScale.y - abs(cubeCoordPoint.y);
	// ������ �ʴ´ٸ�
	if (penetration < 0)
		return false;
	// �����ٸ�
	if (minPenetration > penetration)
	{
		minPenetration = penetration;
		if (cubeCoordPoint.y >= 0)
			normal = object1.transform.GetUp();
		else
			normal = -object1.transform.GetUp();
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
			normal = object1.transform.GetForward();
		else
			normal = -object1.transform.GetForward();
	}

	// �̰� ��¦ �̻���.. �ݴ��� ��ü�� ���ٴ°�..
	// 20220728_�ϴ� �ݴ��� ��ü �ִ°ɷ� ��� ���ļ� ������.. ���� �ڵ� �̰���
	// contact.object1 = nullptr;
	// contact.object2 = &object1;
	
	Contact contact;
	contact.object1 = &object2;
	contact.object2 = &object1;
	contact.point = point;
	contact.normal = normal;
	contact.penetration = minPenetration;
	contacts.push_back(contact);
}

// ť�� �� �� �浹 ������ Ȯ�ο� ���
bool CubeAndEdge(Object& object, LineSegment edge, vector<Contact>& contacts)
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
	float x, y, a, b, c, lengthToClosestPointOnCubeEdge, lengthToClosestPointOnEdge;

	Vector3 contactPointOnCubeEdge;
	Vector3 contactPointOnEdge;
	float minPenetration;

	cubeEdges.push_back(LineSegment(object.transform.GetPosition() + (object.transform.GetRight() + object.transform.GetUp() + object.transform.GetForward()) / 2.0f, -object.transform.GetRight(), object.transform.GetScale().x));
	cubeEdges.push_back(LineSegment(object.transform.GetPosition() + (-object.transform.GetRight() + object.transform.GetUp() + object.transform.GetForward()) / 2.0f, -object.transform.GetForward(), object.transform.GetScale().z));
	cubeEdges.push_back(LineSegment(object.transform.GetPosition() + (-object.transform.GetRight() + object.transform.GetUp() - object.transform.GetForward()) / 2.0f, object.transform.GetRight(), object.transform.GetScale().x));
	cubeEdges.push_back(LineSegment(object.transform.GetPosition() + (object.transform.GetRight() + object.transform.GetUp() - object.transform.GetForward()) / 2.0f, object.transform.GetForward(), object.transform.GetScale().z));
	cubeEdges.push_back(LineSegment(object.transform.GetPosition() + (object.transform.GetRight() + object.transform.GetUp() + object.transform.GetForward()) / 2.0f, -object.transform.GetUp(), object.transform.GetScale().y));
	cubeEdges.push_back(LineSegment(object.transform.GetPosition() + (-object.transform.GetRight() + object.transform.GetUp() + object.transform.GetForward()) / 2.0f, -object.transform.GetUp(), object.transform.GetScale().y));
	cubeEdges.push_back(LineSegment(object.transform.GetPosition() + (-object.transform.GetRight() + object.transform.GetUp() - object.transform.GetForward()) / 2.0f, -object.transform.GetUp(), object.transform.GetScale().y));
	cubeEdges.push_back(LineSegment(object.transform.GetPosition() + (object.transform.GetRight() + object.transform.GetUp() - object.transform.GetForward()) / 2.0f, -object.transform.GetUp(), object.transform.GetScale().y));
	cubeEdges.push_back(LineSegment(object.transform.GetPosition() + (object.transform.GetRight() - object.transform.GetUp() + object.transform.GetForward()) / 2.0f, -object.transform.GetRight(), object.transform.GetScale().x));
	cubeEdges.push_back(LineSegment(object.transform.GetPosition() + (-object.transform.GetRight() - object.transform.GetUp() + object.transform.GetForward()) / 2.0f, -object.transform.GetForward(), object.transform.GetScale().z));
	cubeEdges.push_back(LineSegment(object.transform.GetPosition() + (-object.transform.GetRight() - object.transform.GetUp() - object.transform.GetForward()) / 2.0f, object.transform.GetRight(), object.transform.GetScale().x));
	cubeEdges.push_back(LineSegment(object.transform.GetPosition() + (object.transform.GetRight() - object.transform.GetUp() - object.transform.GetForward()) / 2.0f, object.transform.GetForward(), object.transform.GetScale().z));
	for (int i = 0; i < cubeEdges.size(); i++)
	{
		// ť���� �� ���а� ���Ϸ��� ���� �� �ִܰŸ� ���(https://wizardmania.tistory.com/21)
		x = Vector3::Dot(cubeEdges[i].origin - edge.origin, cubeEdges[i].direction);
		y = Vector3::Dot(cubeEdges[i].origin - edge.origin, edge.direction);
		a = Vector3::Dot(cubeEdges[i].direction, cubeEdges[i].direction);
		b = Vector3::Dot(edge.direction, edge.direction);
		c = Vector3::Dot(cubeEdges[i].direction, edge.direction);
		
		// �� ��� �� ������ ������
		if (c * c - a * b == 0)
		{
			// �˾Ƽ� ����ó��
			// �� ��쿡�� �ٸ� �鿡 �ִ� �����̳� ������ �и� �浹 �ҰŰ� �׳� �Ѿ�� �ǰ�����
			// �츮�� ��, ��, �� ��� �Ϻ��� ��ġ�� ��츦 ó������ �ʴ´ٸ� �� ��𿡼��� ó������ ���� ��
			// ¶�� �갡 �и� �Ǽ� 0���� ������ �Ǵϱ� return�� �ϴ��� �� ó���� �������
		}
		lengthToClosestPointOnCubeEdge = (x * b - y * c) / (c * c - a * b);
		lengthToClosestPointOnEdge = (x * c - y * a) / (c * c - a * b);
		closestPointOnCubeEdge = cubeEdges[i].origin + cubeEdges[i].direction * lengthToClosestPointOnCubeEdge;
		closestPointOnEdge = edge.origin + edge.direction * lengthToClosestPointOnEdge;
		
		// �� ��� �ִܰŸ��� ���� ������ �ۿ� �ִ°���
		if (lengthToClosestPointOnCubeEdge < 0 || lengthToClosestPointOnCubeEdge > cubeEdges[i].length ||
			lengthToClosestPointOnEdge < 0 || lengthToClosestPointOnEdge > edge.length)
		{
			// �˾Ƽ� ����ó��
			// ���ɴ�� ���� ���ϴ°ɷ� ������..
			// ¶�� �굵 ó�� �������
		}
		penetration = Vector3::Magnitude(closestPointOnCubeEdge - closestPointOnEdge);
		
		// �� ��� ť�� �������� ť���� �������� �ٸ� ������ �� �����Ƿ� �浹�� ����
		if (Vector3::SquareMagnitude(object.transform.GetPosition() - closestPointOnCubeEdge) > Vector3::SquareMagnitude(object.transform.GetPosition() - closestPointOnEdge))
		{
			// �׷��ٸ� ��ǻ� ť���� ��� ������ �� �浹�Ȱɷ� ���ðŶ� ���� ��� �浹�� ���� ��󳾰���
			// 
			if (i == 0)
			{
				minPenetration = penetration;
				contactPointOnCubeEdge = closestPointOnCubeEdge;
				contactPointOnEdge = closestPointOnEdge;
			}
			else
			{
				if (penetration < minPenetration)
				{
					minPenetration = penetration;
					contactPointOnCubeEdge = closestPointOnCubeEdge;
					contactPointOnEdge = closestPointOnEdge;
				}
			}
		}
	}

	// �̰� ��¦ �̻���.. �ݴ��� ��ü�� ���ٴ°�..
	Contact contact;
	contact.object1 = nullptr;
	contact.object2 = &object;
	contact.point = contactPointOnEdge;
	contact.normal = Vector3::Normalize(contactPointOnCubeEdge - contactPointOnEdge);
	contact.penetration = minPenetration;
	contacts.push_back(contact);
	
	return true;
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
	vector<Vector3> cube2Vertices;
	cube2Vertices.push_back(Vector3(object2->transform.GetPosition() + (object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y + object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f));
	cube2Vertices.push_back(Vector3(object2->transform.GetPosition() + (-object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y + object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f));
	cube2Vertices.push_back(Vector3(object2->transform.GetPosition() + (-object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y - object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f));
	cube2Vertices.push_back(Vector3(object2->transform.GetPosition() + (object2->transform.GetRight() * object2->transform.GetScale().x + object2->transform.GetUp() * object2->transform.GetScale().y - object2->transform.GetForward() * object2->transform.GetScale().z) / 2.0f));
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
		CubeAndPoint(*object1, *object2, cube2Vertices[i], contacts);
	}
	return true;
}