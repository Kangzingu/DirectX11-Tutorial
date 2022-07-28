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
	// 이거 square magnitude 로 바꿔서 루트 씌우는 연산 줄이자 나중에
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
	Vector3 cubeHalfScale = object2->transform.GetScale() / 2.0f;
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
	Vector3 cubeHalfScale = object1->transform.GetScale() / 2.0f;
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
// 큐브 간 단순 충돌여부 확인에 사용
float ProjectObjectToAxis(Object& object, Vector3 axis)
{
	return abs(Vector3::Dot(object.transform.GetRight() * object.transform.GetScale().x, axis)) +
		abs(Vector3::Dot(object.transform.GetUp() * object.transform.GetScale().y, axis)) +
		abs(Vector3::Dot(object.transform.GetForward() * object.transform.GetScale().z, axis));
}
// 큐브 간 상세 충돌 데이터 확인에 사용
bool CubeAndPoint(Object& object1, Object& object2, Vector3 point, vector<Contact>& contacts)
{
	Vector3 cubeCoordPoint = object1.transform.GetRotationMatrix().Inverse() * object1.transform.GetTranslationMatrix().Inverse() * point;
	Vector3 cubeHalfScale = object1.transform.GetScale() / 2.0f;
	Vector3 normal;
	float penetration;
	float minPenetration;

	penetration = cubeHalfScale.x - abs(cubeCoordPoint.x);
	// 만나지 않는다면
	if (penetration < 0)
		return false;
	// 만난다면
	minPenetration = penetration;
	if (cubeCoordPoint.x >= 0)
		normal = object1.transform.GetRight();
	else
		normal = -object1.transform.GetRight();

	penetration = cubeHalfScale.y - abs(cubeCoordPoint.y);
	// 만나지 않는다면
	if (penetration < 0)
		return false;
	// 만난다면
	if (minPenetration > penetration)
	{
		minPenetration = penetration;
		if (cubeCoordPoint.y >= 0)
			normal = object1.transform.GetUp();
		else
			normal = -object1.transform.GetUp();
	}

	penetration = cubeHalfScale.z - abs(cubeCoordPoint.z);
	// 만나지 않는다면
	if (penetration < 0)
		return false;
	// 만난다면
	if (minPenetration > penetration)
	{
		minPenetration = penetration;
		if (cubeCoordPoint.z >= 0)
			normal = object1.transform.GetForward();
		else
			normal = -object1.transform.GetForward();
	}

	// 이거 살짝 이상함.. 반대편 물체가 없다는겡..
	// 20220728_일단 반대편 물체 있는걸로 잠시 고쳐서 실험중.. 원래 코드 이거임
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

// 큐브 간 상세 충돌 데이터 확인에 사용
bool CubeAndEdge(Object& object, LineSegment edge, vector<Contact>& contacts)
{
	//    2 ------- 3
	//   /|        /|
	//  1 ------- 0 7
	// 	|/        |/
	// 	5 ------- 4
	// 시계방향으로 총 12개 선분
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
		// 큐브의 한 선분과 비교하려는 선분 간 최단거리 계산(https://wizardmania.tistory.com/21)
		x = Vector3::Dot(cubeEdges[i].origin - edge.origin, cubeEdges[i].direction);
		y = Vector3::Dot(cubeEdges[i].origin - edge.origin, edge.direction);
		a = Vector3::Dot(cubeEdges[i].direction, cubeEdges[i].direction);
		b = Vector3::Dot(edge.direction, edge.direction);
		c = Vector3::Dot(cubeEdges[i].direction, edge.direction);
		
		// 이 경우 두 선분이 평행임
		if (c * c - a * b == 0)
		{
			// 알아서 예외처리
			// 이 경우에는 다른 면에 있는 선분이나 점에서 분명 충돌 할거고 그냥 넘어가도 되겠지만
			// 우리가 점, 선, 면 모두 완벽히 겹치는 경우를 처리하지 않는다면 그 어디에서도 처리되지 않을 듯
			// 쨋든 얘가 분모가 되서 0으로 나누게 되니까 return을 하던지 꼭 처리는 해줘야함
		}
		lengthToClosestPointOnCubeEdge = (x * b - y * c) / (c * c - a * b);
		lengthToClosestPointOnEdge = (x * c - y * a) / (c * c - a * b);
		closestPointOnCubeEdge = cubeEdges[i].origin + cubeEdges[i].direction * lengthToClosestPointOnCubeEdge;
		closestPointOnEdge = edge.origin + edge.direction * lengthToClosestPointOnEdge;
		
		// 이 경우 최단거리가 선분 범위의 밖에 있는것임
		if (lengthToClosestPointOnCubeEdge < 0 || lengthToClosestPointOnCubeEdge > cubeEdges[i].length ||
			lengthToClosestPointOnEdge < 0 || lengthToClosestPointOnEdge > edge.length)
		{
			// 알아서 예외처리
			// 일케대면 교차 안하는걸로 추정댐..
			// 쨋든 얘도 처리 해줘야함
		}
		penetration = Vector3::Magnitude(closestPointOnCubeEdge - closestPointOnEdge);
		
		// 이 경우 큐브 중점에서 큐브의 엣지보다 다른 엣지가 더 가까우므로 충돌이 맞음
		if (Vector3::SquareMagnitude(object.transform.GetPosition() - closestPointOnCubeEdge) > Vector3::SquareMagnitude(object.transform.GetPosition() - closestPointOnEdge))
		{
			// 그렇다면 사실상 큐브의 모든 엣지가 다 충돌된걸로 나올거라 가장 얇게 충돌된 넘을 골라낼거임
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

	// 이거 살짝 이상함.. 반대편 물체가 없다는겡..
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
	// 투영하고 체크해봐야 하는 축은 총 15개로 다음과 같음
	// Cube1의 x, y, z 축 3개
	// Cube2의 x, y, z 축 3개
	// Cbue1의 x, y, z 축과 Cube2의 x, y, z 축을 각각 조합 후 외적으로 만든 Cross(xx), Cross(xy), Cross(xz), Cross(yx), Cross(yy), Cross(yz), Cross(zx), Cross(zy), Cross(zz) 축 9개	
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
	// 1차 검사 통과! 여기까지 왔다는건 접촉하고 있다는 것임(눈으로 확인함), 이제 상세하게 어디서 얼만큼 충돌했는지 찾는것임
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