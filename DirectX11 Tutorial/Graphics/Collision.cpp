#include "Object.h"
#include "Collision.h"
/************************************/
/* 물체-1 이 물체-2 에 가서 박는것! */
/************************************/

void Collision::LayAndSphere(Vector3& layPosition, Vector3& layDirection, Object* object)
{
	Vector3 layToSphere = object->transform.GetPosition() - layPosition;
	// lay(카메라)의 원점이 구 안에 있는 경우. 상황에 따라 알아서 처리하도록..
	if (Vector3::SquareMagnitude(layToSphere) < object->sphereCollider.GetRadius() * object->sphereCollider.GetRadius())
	{
	}
	float layToSphereDotLayDirectionResult = Vector3::Dot(layToSphere, layDirection);
	// lay(카메라)의 진행방향에 구가 있는 경우. 후면에 구가 있으면 1차적으로 걸러짐
	if (layToSphereDotLayDirectionResult >= 0)
	{
		float layAndSphereLeastLengthSquare = Vector3::SquareMagnitude(layToSphere) - layToSphereDotLayDirectionResult * layToSphereDotLayDirectionResult;
		// lay와 구 원점간의 최소 거리가 반지름보다 작은 경우. 크면 둘이 닿을 수가 없어 2차적으로 걸러짐
		if (layAndSphereLeastLengthSquare <= object->sphereCollider.GetRadius() * object->sphereCollider.GetRadius())
		{
			// 여기부턴 일단 충돌이 있긴 하니까 충돌 위치를 구하는거임
			layDirection = Vector3::Normalize(layDirection);// 혹시 모르니까..
			float t = sqrt(Vector3::SquareMagnitude(layToSphere) - layAndSphereLeastLengthSquare);
			float halfContactPointsGap = sqrt(object->sphereCollider.GetRadius() * object->sphereCollider.GetRadius() - layAndSphereLeastLengthSquare);

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
	// 점과 평면의 거리 공식 + 점이 아니라 구니까 radius도 고려한거임
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
	// 점과 평면의 거리 공식 + 점이 아니라 구니까 radius도 고려한거임
	if (centerDistance < object1->sphereCollider.GetRadius() && centerDistance > -object1->sphereCollider.GetRadius())
	{
		Contact contact;
		contact.object1 = object1;
		contact.object2 = object2;
		contact.point = object1->transform.GetPosition() - object2->planeCollider.GetNormal() * centerDistance;
		contact.normal = object2->planeCollider.GetNormal();
		contact.penetration = centerDistance - object1->sphereCollider.GetRadius();
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
	if (abs(cubeCoordSphereCenterPosition.x) - object1->sphereCollider.GetRadius() < cubeHalfScale.x &&
		abs(cubeCoordSphereCenterPosition.y) - object1->sphereCollider.GetRadius() < cubeHalfScale.y &&
		abs(cubeCoordSphereCenterPosition.z) - object1->sphereCollider.GetRadius() < cubeHalfScale.z)
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
		if (Vector3::Magnitude(cubeCoordSphereCenterPosition - closestPoint) <= object1->sphereCollider.GetRadius())
		{
			Vector3 worldCoordClosestPoint = object2->transform.GetWorldMatrix() * closestPoint;
			Contact contact;
			contact.object1= object1;
			contact.object2 = object2;
			contact.point = worldCoordClosestPoint;
			// 이거 중점을 넘어서 더 겹쳐버리면 계산이 아예 반대로 될듯, 관통해버린걸 해결하는거에 포함해서 해결해야하는 느낌?
			contact.normal = Vector3::Normalize(object1->transform.GetPosition() - worldCoordClosestPoint);
			contact.penetration = object1->sphereCollider.GetRadius() - Vector3::Magnitude(object1->transform.GetPosition() - worldCoordClosestPoint);
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
	Vector3 cubeCoordPoint = object2.transform.GetRotationMatrix().Inverse() * object2.transform.GetTranslationMatrix().Inverse() * point;
	Vector3 cubeHalfScale = object2.transform.GetScale() / 2.0f;
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
		normal = object2.transform.GetRight();
	else
		normal = -object2.transform.GetRight();

	penetration = cubeHalfScale.y - abs(cubeCoordPoint.y);
	// 만나지 않는다면
	if (penetration < 0)
		return false;
	// 만난다면
	if (minPenetration > penetration)
	{
		minPenetration = penetration;
		if (cubeCoordPoint.y >= 0)
			normal = object2.transform.GetUp();
		else
			normal = -object2.transform.GetUp();
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
			normal = object2.transform.GetForward();
		else
			normal = -object2.transform.GetForward();
	}

	// 이거 살짝 이상함.. 반대편 물체가 없다는겡..
	// 20220728_일단 반대편 물체 있는걸로 잠시 고쳐서 실험중.. 원래 코드 이거임
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
// 큐브 간 상세 충돌 데이터 확인에 사용
void CubeAndEdge(Object& object1, Object& object2, LineSegment edge, vector<Contact>& contacts)
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
		// 큐브의 한 선분과 비교하려는 선분 간 최단거리 계산(Real Time Rendering 2 675pg)
		// Line Segment1 = o1 + s * d1
		// Line Segment2 = o2 + t + d2
		Vector3 o1 = cubeEdges[i].origin;
		Vector3 o2 = edge.origin;
		Vector3 d1 = cubeEdges[i].direction;
		Vector3 d2 = edge.direction;
		float s, t;
		if (Vector3::Cross(d1, d2) == Vector3::Zero())
		{
			// 이 경우 두 직선이 평행인 것
			continue;
		}
		s = Vector3::Dot(Vector3::Cross((o2 - o1), d2), Vector3::Cross(d1, d2)) / Vector3::Dot(Vector3::Cross(d1, d2), Vector3::Cross(d1, d2));
		t = Vector3::Dot(Vector3::Cross((o1 - o2), d1), Vector3::Cross(d2, d1)) / Vector3::Dot(Vector3::Cross(d2, d1), Vector3::Cross(d2, d1));
		if (s<0 || s>cubeEdges[i].length ||
			t<0 || t>edge.length)
		{
			continue;
			// 이 경우 교차하지 않음
		}
		closestPointOnCubeEdge = cubeEdges[i].origin + cubeEdges[i].direction * s;
		closestPointOnEdge = edge.origin + edge.direction * t;

		// 이게 박스가 한없이 넓은거(사이즈가 좀 한쪽에 치우친거)로 하면 결과가 넘 이상해짐.. 다른 방법 없을깡
		// 쨋든 이거 고쳐야함 closestPointOnCubeEdge - closestPointOnEdge 가 아니고 object2.transform.GetPosition() - closestPointOnEdge 이런식으로 하던가
		// 이런식이라면 결국 점 충돌검사 방식보다 우선순위가 낮아질 듯. 불안불안한 부분이 많은거같음

		penetration = Vector3::Magnitude(closestPointOnCubeEdge - closestPointOnEdge);
		// 이 경우 큐브 중점에서 큐브의 엣지보다 다른 엣지가 더 가까우므로 충돌이 맞음
		if (Vector3::SquareMagnitude(object2.transform.GetPosition() - closestPointOnCubeEdge) > Vector3::SquareMagnitude(object2.transform.GetPosition() - closestPointOnEdge))
		{
			// 그렇다면 사실상 큐브의 모든 엣지가 다 충돌된걸로 나올거라 가장 얇게 충돌된 넘을 골라낼거임
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
	// 이거 살짝 이상함.. 반대편 물체가 없다는겡..
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
	// 303pg 읽으면 댐
}