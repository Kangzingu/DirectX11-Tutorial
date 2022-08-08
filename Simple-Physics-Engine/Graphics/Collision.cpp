#include "Object.h"
#include "Collision.h"
/************************************/
/* 물체-1 이 물체-2 에 가서 박는것! */
/************************************/

struct Projection
{
	float min;
	float max;
};
Projection ProjectVerticesToAxis(Object& object, Vector3& axis)
{
	float point;
	Projection result;
	for (int i = 0; i < object.model.meshes.size(); i++)
	{
		Mesh mesh = object.model.meshes[i];
		for (int j = 0; j < mesh.vertices.size(); j++)
		{
			point = Vector3::Dot(axis, object.transform.GetWorldMatrix() * mesh.vertices[j].pos);
			if (i == 0 && j == 0)
			{
				result.min = point;
				result.max = point;
				continue;
			}

			if (point < result.min)
				result.min = point;
			else if (point > result.max)
				result.max = point;
		}
	}
	return result;
}
float GetOverlappedAmount(Projection p1, Projection p2)
{
	//	p1	p2	p1	p2
	if (p2.min < p1.max && p2.max >= p1.max)
	{
		return p1.max - p2.min;
	}
	//	p2	p1	p2	p1
	else if (p1.min < p2.max && p1.max >= p2.max)
	{
		return p2.max - p1.min;
	}
	//	p1	p2	p2	p1
	else if (p1.min <= p2.min && p1.max >= p2.max)
	{
		return min(p1.max - p2.min, p2.max - p1.min);
	}
	//	p2	p1	p1	p2
	else if (p2.min <= p1.min && p2.max >= p1.max)
	{
		return min(p2.max - p1.min, p1.max - p2.min);
	}
	// 겹치지 않음
	else
	{
		return 0;
	}
}
bool Collision::BroadPhaseBoundingSphere(Object& object1, Object& object2, vector<Vector3>* lineForDebug)
{
	// 구 충돌체 만들기
	float maxDistance=0;
	float distance;
	float object1Radius;
	float object2Radius;
	for (int i = 0; i < object1.model.meshes.size(); i++)
	{
		for (UINT j = 0; j < object1.model.meshes[i].vertices.size(); j++)
		{
			distance = Vector3::SquareMagnitude(Vector3(object1.model.meshes[i].vertices[j].pos.x * object1.transform.GetScale().x,
												   object1.model.meshes[i].vertices[j].pos.y * object1.transform.GetScale().y,
												   object1.model.meshes[i].vertices[j].pos.z * object1.transform.GetScale().z));
			if (distance > maxDistance)
			{
				maxDistance = distance;
			}
		}
	}
	object1Radius = maxDistance;
	maxDistance = 0;
	for (int i = 0; i < object2.model.meshes.size(); i++)
	{
		for (UINT j = 0; j < object2.model.meshes[i].vertices.size(); j++)
		{
			distance = Vector3::SquareMagnitude(Vector3(object2.model.meshes[i].vertices[j].pos.x * object2.transform.GetScale().x,
												   object2.model.meshes[i].vertices[j].pos.y * object2.transform.GetScale().y,
												   object2.model.meshes[i].vertices[j].pos.z * object2.transform.GetScale().z));
			if (distance > maxDistance)
			{
				maxDistance = distance;
			}
		}
	}
	object2Radius = maxDistance;
	
	float objectsDistance=Vector3::Magnitude(object1.transform.GetPosition() - object2.transform.GetPosition());

	if (objectsDistance > sqrt(object1Radius) + sqrt(object2Radius))
		return false;

	return true;
}

bool Collision::BroadPhaseAxisAlignBoundingBox(Object& object1, Object& object2, vector<Vector3>* lineForDebug)
{
	// AABB 충돌체 만들기
	float maxLen = 0;
	float len;
	Vector3 object1MinPosition, object1MaxPosition;
	Vector3 object2MinPosition, object2MaxPosition;
	Vector3 object1VertexWorldPos, object2VertexWorldPos;
			object1MinPosition = object1.transform.GetWorldMatrix() * object1MinPosition;
			object1MaxPosition = object1.transform.GetWorldMatrix() * object1MaxPosition;
	for (int i = 0; i < object1.model.meshes.size(); i++)
	{
		for (UINT j = 0; j < object1.model.meshes[i].vertices.size(); j++)
		{
			object1VertexWorldPos = object1.transform.GetWorldMatrix() * Vector3(object1.model.meshes[i].vertices[j].pos.x,
																				 object1.model.meshes[i].vertices[j].pos.y,
																				 object1.model.meshes[i].vertices[j].pos.z);
			if (object1VertexWorldPos.x < object1MinPosition.x)
			{
				object1MinPosition.x = object1VertexWorldPos.x;
			}
			if (object1VertexWorldPos.y < object1MinPosition.y)
			{
				object1MinPosition.y = object1VertexWorldPos.y;
			}
			if (object1VertexWorldPos.z < object1MinPosition.z)
			{
				object1MinPosition.z = object1VertexWorldPos.z;
			}
			if (object1VertexWorldPos.x > object1MaxPosition.x)
			{
				object1MaxPosition.x = object1VertexWorldPos.x;
			}
			if (object1VertexWorldPos.y > object1MaxPosition.y)
			{
				object1MaxPosition.y = object1VertexWorldPos.y;
			}
			if (object1VertexWorldPos.z > object1MaxPosition.z)
			{
				object1MaxPosition.z = object1VertexWorldPos.z;
			}
		}
	}
	object2MinPosition = object2.transform.GetWorldMatrix() * object2MinPosition;
	object2MaxPosition = object2.transform.GetWorldMatrix() * object2MaxPosition;
	for (int i = 0; i < object2.model.meshes.size(); i++)
	{
		for (UINT j = 0; j < object2.model.meshes[i].vertices.size(); j++)
		{
			object2VertexWorldPos = object2.transform.GetWorldMatrix() * Vector3(object2.model.meshes[i].vertices[j].pos.x,
																				 object2.model.meshes[i].vertices[j].pos.y,
																				 object2.model.meshes[i].vertices[j].pos.z);
			if (object2VertexWorldPos.x < object2MinPosition.x)
			{
				object2MinPosition.x = object2VertexWorldPos.x;
			}
			if (object2VertexWorldPos.y < object2MinPosition.y)
			{
				object2MinPosition.y = object2VertexWorldPos.y;
			}
			if (object2VertexWorldPos.z < object2MinPosition.z)
			{
				object2MinPosition.z = object2VertexWorldPos.z;
			}
			if (object2VertexWorldPos.x > object2MaxPosition.x)
			{
				object2MaxPosition.x = object2VertexWorldPos.x;
			}
			if (object2VertexWorldPos.y > object2MaxPosition.y)
			{
				object2MaxPosition.y = object2VertexWorldPos.y;
			}
			if (object2VertexWorldPos.z > object2MaxPosition.z)
			{
				object2MaxPosition.z = object2VertexWorldPos.z;
			}
		}
	}
	lineForDebug[0].push_back(object1MinPosition);
	lineForDebug[1].push_back(object1MaxPosition);
	if (object1MinPosition.x > object2MaxPosition.x || object1MaxPosition.x < object2MinPosition.x ||
		object1MinPosition.y > object2MaxPosition.y || object1MaxPosition.y < object2MinPosition.y ||
		object1MinPosition.z > object2MaxPosition.z || object1MaxPosition.z < object2MinPosition.z)
	{
		return false;
	}
	return true;
}

void Collision::NarrowPhaseSphereAndSphere(Object* object1, Object* object2, vector<Contact>& contacts)
{
	Vector3 centerDiff = object1->transform.GetPosition() - object2->transform.GetPosition();
	// 이거 square magnitude 로 바꿔서 루트 씌우는 연산 줄이자 나중에
	float centerDistance = Vector3::Magnitude(centerDiff);
	if (centerDistance < object1->sphereCollider.GetRadius() + object2->sphereCollider.GetRadius())
	{
		Contact contact;
		contact.m_objects[0] = object1;
		contact.m_objects[1] = object2;
		contact.m_point = object2->transform.GetPosition() + (centerDiff / 2.0f);
		contact.m_normal = Vector3::Normalize(centerDiff);
		contact.m_penetration = object1->sphereCollider.GetRadius() + object2->sphereCollider.GetRadius() - centerDistance;
		contacts.push_back(contact);
	}
}
void Collision::NarrowPhaseSphereAndCube(Object* object1, Object* object2, vector<Contact>& contacts)
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
			contact.m_objects[0]= object1;
			contact.m_objects[1] = object2;
			contact.m_point = worldCoordClosestPoint;
			// 이거 중점을 넘어서 더 겹쳐버리면 계산이 아예 반대로 될듯, 관통해버린걸 해결하는거에 포함해서 해결해야하는 느낌?
			contact.m_normal = Vector3::Normalize(object1->transform.GetPosition() - worldCoordClosestPoint);
			contact.m_penetration = object1->sphereCollider.GetRadius() - Vector3::Magnitude(object1->transform.GetPosition() - worldCoordClosestPoint);
			contacts.push_back(contact);
		}
	}
	// 287pg 읽으면 댐
}
// 큐브 간 상세 충돌 데이터 확인에 사용
void CubeAndVertex(Object& object1, Object& object2, Contact* contact, vector<Vector3>* lineForDebug)
{
	vector<Vector3> vertices;
	vector<Vector3> cubeCoordVertices;
	vertices.push_back(Vector3(object1.transform.GetPosition() + (object1.transform.GetRight() * object1.transform.GetScale().x + object1.transform.GetUp() * object1.transform.GetScale().y + object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object1.transform.GetPosition() + (-object1.transform.GetRight() * object1.transform.GetScale().x + object1.transform.GetUp() * object1.transform.GetScale().y + object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object1.transform.GetPosition() + (-object1.transform.GetRight() * object1.transform.GetScale().x + object1.transform.GetUp() * object1.transform.GetScale().y - object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object1.transform.GetPosition() + (object1.transform.GetRight() * object1.transform.GetScale().x + object1.transform.GetUp() * object1.transform.GetScale().y - object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object1.transform.GetPosition() + (object1.transform.GetRight() * object1.transform.GetScale().x - object1.transform.GetUp() * object1.transform.GetScale().y + object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object1.transform.GetPosition() + (-object1.transform.GetRight() * object1.transform.GetScale().x - object1.transform.GetUp() * object1.transform.GetScale().y + object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object1.transform.GetPosition() + (-object1.transform.GetRight() * object1.transform.GetScale().x - object1.transform.GetUp() * object1.transform.GetScale().y - object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object1.transform.GetPosition() + (object1.transform.GetRight() * object1.transform.GetScale().x - object1.transform.GetUp() * object1.transform.GetScale().y - object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f));

	for (int i = 0; i < vertices.size(); i++)
	{
		cubeCoordVertices.push_back(object2.transform.GetRotationMatrix().Inverse() * object2.transform.GetTranslationMatrix().Inverse() * vertices[i]);
	}

	Vector3 cubeHalfScale = object2.transform.GetScale() / 2.0f;
	Vector3 normal;
	float penetration[3];
	float minPenetration = Vector3::Magnitude(object1.transform.GetScale()) + Vector3::Magnitude(object2.transform.GetScale());
	Vector3 minPenetrationVertex;

	for (int i = 0; i < cubeCoordVertices.size(); i++)
	{
		penetration[0] = cubeHalfScale.x - abs(cubeCoordVertices[i].x);
		penetration[1] = cubeHalfScale.y - abs(cubeCoordVertices[i].y);
		penetration[2] = cubeHalfScale.z - abs(cubeCoordVertices[i].z);
		if (penetration[0] > 0 && penetration[1] > 0 && penetration[2] > 0)
		{
			for (int j = 0; j < 3; j++)
			{
				if (penetration[j] < minPenetration)
				{
					minPenetration = penetration[j];
					minPenetrationVertex = vertices[i];
					switch (j)
					{
					case 0:
						if (cubeCoordVertices[i].x >= 0)
							normal = object2.transform.GetRight();
						else
							normal = -object2.transform.GetRight();
						break;
					case 1:
						if (cubeCoordVertices[i].y >= 0)
							normal = object2.transform.GetUp();
						else
							normal = -object2.transform.GetUp();
						break;
					case 2:
						if (cubeCoordVertices[i].z >= 0)
							normal = object2.transform.GetForward();
						else
							normal = -object2.transform.GetForward();
						break;
					}

				}
			}
		}
	}

	//lineForDebug[0].push_back(minPenetrationVertex);
	//lineForDebug[1].push_back(minPenetrationVertex + (Vector3::Normalize(normal) * minPenetration / 2));
	contact->m_objects[0] = &object1;
	contact->m_objects[1] = &object2;
	contact->m_point = minPenetrationVertex + (Vector3::Normalize(normal) * minPenetration / 2);
	contact->m_normal = Vector3::Normalize(normal);
	contact->m_penetration = minPenetration;
}
// 큐브 간 상세 충돌 데이터 확인에 사용
void CubeAndEdge(Object& object1, Object& object2, Contact* contact, vector<Vector3>* lineForDebug)
{
	// TODO: Object1, 2 바껴있다 수정하자

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
	vector<LineSegment> cube1Edges;
	cube1Edges.push_back(LineSegment(object1.transform.GetPosition() + (object1.transform.GetRight() * object1.transform.GetScale().x + object1.transform.GetUp() * object1.transform.GetScale().y + object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f, -object1.transform.GetRight(), object1.transform.GetScale().x));
	cube1Edges.push_back(LineSegment(object1.transform.GetPosition() + (-object1.transform.GetRight() * object1.transform.GetScale().x + object1.transform.GetUp() * object1.transform.GetScale().y + object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f, -object1.transform.GetForward(), object1.transform.GetScale().z));
	cube1Edges.push_back(LineSegment(object1.transform.GetPosition() + (-object1.transform.GetRight() * object1.transform.GetScale().x + object1.transform.GetUp() * object1.transform.GetScale().y - object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f, object1.transform.GetRight(), object1.transform.GetScale().x));
	cube1Edges.push_back(LineSegment(object1.transform.GetPosition() + (object1.transform.GetRight() * object1.transform.GetScale().x + object1.transform.GetUp() * object1.transform.GetScale().y - object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f, object1.transform.GetForward(), object1.transform.GetScale().z));
	cube1Edges.push_back(LineSegment(object1.transform.GetPosition() + (object1.transform.GetRight() * object1.transform.GetScale().x + object1.transform.GetUp() * object1.transform.GetScale().y + object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f, -object1.transform.GetUp(), object1.transform.GetScale().y));
	cube1Edges.push_back(LineSegment(object1.transform.GetPosition() + (-object1.transform.GetRight() * object1.transform.GetScale().x + object1.transform.GetUp() * object1.transform.GetScale().y + object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f, -object1.transform.GetUp(), object1.transform.GetScale().y));
	cube1Edges.push_back(LineSegment(object1.transform.GetPosition() + (-object1.transform.GetRight() * object1.transform.GetScale().x + object1.transform.GetUp() * object1.transform.GetScale().y - object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f, -object1.transform.GetUp(), object1.transform.GetScale().y));
	cube1Edges.push_back(LineSegment(object1.transform.GetPosition() + (object1.transform.GetRight() * object1.transform.GetScale().x + object1.transform.GetUp() * object1.transform.GetScale().y - object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f, -object1.transform.GetUp(), object1.transform.GetScale().y));
	cube1Edges.push_back(LineSegment(object1.transform.GetPosition() + (object1.transform.GetRight() * object1.transform.GetScale().x - object1.transform.GetUp() * object1.transform.GetScale().y + object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f, -object1.transform.GetRight(), object1.transform.GetScale().x));
	cube1Edges.push_back(LineSegment(object1.transform.GetPosition() + (-object1.transform.GetRight() * object1.transform.GetScale().x - object1.transform.GetUp() * object1.transform.GetScale().y + object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f, -object1.transform.GetForward(), object1.transform.GetScale().z));
	cube1Edges.push_back(LineSegment(object1.transform.GetPosition() + (-object1.transform.GetRight() * object1.transform.GetScale().x - object1.transform.GetUp() * object1.transform.GetScale().y - object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f, object1.transform.GetRight(), object1.transform.GetScale().x));
	cube1Edges.push_back(LineSegment(object1.transform.GetPosition() + (object1.transform.GetRight() * object1.transform.GetScale().x - object1.transform.GetUp() * object1.transform.GetScale().y - object1.transform.GetForward() * object1.transform.GetScale().z) / 2.0f, object1.transform.GetForward(), object1.transform.GetScale().z));

	vector<LineSegment> cube2Edges;
	cube2Edges.push_back(LineSegment(object2.transform.GetPosition() + (object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y + object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetRight(), object2.transform.GetScale().x));
	cube2Edges.push_back(LineSegment(object2.transform.GetPosition() + (-object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y + object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetForward(), object2.transform.GetScale().z));
	cube2Edges.push_back(LineSegment(object2.transform.GetPosition() + (-object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y - object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, object2.transform.GetRight(), object2.transform.GetScale().x));
	cube2Edges.push_back(LineSegment(object2.transform.GetPosition() + (object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y - object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, object2.transform.GetForward(), object2.transform.GetScale().z));
	cube2Edges.push_back(LineSegment(object2.transform.GetPosition() + (object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y + object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetUp(), object2.transform.GetScale().y));
	cube2Edges.push_back(LineSegment(object2.transform.GetPosition() + (-object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y + object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetUp(), object2.transform.GetScale().y));
	cube2Edges.push_back(LineSegment(object2.transform.GetPosition() + (-object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y - object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetUp(), object2.transform.GetScale().y));
	cube2Edges.push_back(LineSegment(object2.transform.GetPosition() + (object2.transform.GetRight() * object2.transform.GetScale().x + object2.transform.GetUp() * object2.transform.GetScale().y - object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetUp(), object2.transform.GetScale().y));
	cube2Edges.push_back(LineSegment(object2.transform.GetPosition() + (object2.transform.GetRight() * object2.transform.GetScale().x - object2.transform.GetUp() * object2.transform.GetScale().y + object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetRight(), object2.transform.GetScale().x));
	cube2Edges.push_back(LineSegment(object2.transform.GetPosition() + (-object2.transform.GetRight() * object2.transform.GetScale().x - object2.transform.GetUp() * object2.transform.GetScale().y + object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, -object2.transform.GetForward(), object2.transform.GetScale().z));
	cube2Edges.push_back(LineSegment(object2.transform.GetPosition() + (-object2.transform.GetRight() * object2.transform.GetScale().x - object2.transform.GetUp() * object2.transform.GetScale().y - object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, object2.transform.GetRight(), object2.transform.GetScale().x));
	cube2Edges.push_back(LineSegment(object2.transform.GetPosition() + (object2.transform.GetRight() * object2.transform.GetScale().x - object2.transform.GetUp() * object2.transform.GetScale().y - object2.transform.GetForward() * object2.transform.GetScale().z) / 2.0f, object2.transform.GetForward(), object2.transform.GetScale().z));

	Vector3 closestPointOnCubeEdge;
	Vector3 closestPointOnEdge;
	float penetration;

	Vector3 contactPointOnCubeEdge;
	Vector3 contactPointOnEdge;
	float minPenetration = Vector3::Magnitude(object1.transform.GetScale()) + Vector3::Magnitude(object2.transform.GetScale());

	for (int i = 0; i < cube1Edges.size(); i++)
	{
		for (int j = 0; j < cube2Edges.size(); j++)
		{
			Vector3 o1 = cube2Edges[j].origin;
			Vector3 o2 = cube1Edges[i].origin;
			Vector3 d1 = cube2Edges[j].direction;
			Vector3 d2 = cube1Edges[i].direction;
			float s, t;
			if (Vector3::Cross(d1, d2) == Vector3::Zero())
			{
				// 이 경우 두 직선이 평행인 것
				continue;
			}
			float x = Vector3::Dot((o1 - o2), d1);
			float y = Vector3::Dot((o1 - o2), d2);
			float a = Vector3::Dot(d1, d1);
			float b = Vector3::Dot(d2, d2);
			float c = Vector3::Dot(d1, d2);
			s = (x * b - y * c) / (c * c - a * b);
			t = (x * c - y * a) / (c * c - a * b);
			if (s<0 || s>cube2Edges[j].length ||
				t<0 || t>cube1Edges[i].length)
			{
				continue;
				// 이 경우 교차하지 않음
			}
			closestPointOnCubeEdge = cube2Edges[j].origin + cube2Edges[j].direction * s;
			closestPointOnEdge = cube1Edges[i].origin + cube1Edges[i].direction * t;

			// 이게 박스가 한없이 넓은거(사이즈가 좀 한쪽에 치우친거)로 하면 결과가 넘 이상해짐.. 다른 방법 없을깡
			// 쨋든 이거 고쳐야함 closestPointOnCubeEdge - closestPointOnEdge 가 아니고 object2.transform.GetPosition() - closestPointOnEdge 이런식으로 하던가
			// 이런식이라면 결국 점 충돌검사 방식보다 우선순위가 낮아질 듯. 불안불안한 부분이 많은거같음

			penetration = Vector3::Magnitude(closestPointOnCubeEdge - closestPointOnEdge);
			if (penetration == 0)
				continue;

			// 이 경우 큐브 중점에서 큐브의 엣지보다 다른 엣지가 더 가까우므로 충돌이 맞음
			if (Vector3::SquareMagnitude(object2.transform.GetPosition() - closestPointOnCubeEdge) > Vector3::SquareMagnitude(object2.transform.GetPosition() - closestPointOnEdge) &&
				Vector3::SquareMagnitude(object1.transform.GetPosition() - closestPointOnEdge) > Vector3::SquareMagnitude(object1.transform.GetPosition() - closestPointOnCubeEdge))
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
	}
	for (int i = 0; i < cube1Edges.size(); i++)
	{
		//lineForDebug[0].push_back(cube1Edges[i].origin);
		//lineForDebug[1].push_back(cube1Edges[i].origin + cube1Edges[i].direction * cube1Edges[i].length);
	}
	for (int i = 0; i < cube2Edges.size(); i++)
	{
		//lineForDebug[0].push_back(cube2Edges[i].origin);
		//lineForDebug[1].push_back(cube2Edges[i].origin + cube2Edges[i].direction * cube2Edges[i].length);
	}
	contact->m_objects[0] = &object1;
	contact->m_objects[1] = &object2;
	contact->m_point = contactPointOnEdge + Vector3::Normalize(contactPointOnCubeEdge - contactPointOnEdge) * minPenetration / 2;
	contact->m_normal = Vector3::Normalize(contactPointOnCubeEdge - contactPointOnEdge);
	contact->m_penetration = minPenetration;
}

// 큐브 간 단순 충돌여부 확인에 사용
Projection ProjectCubeToAxis(Object& object, Vector3& axis)
{
	vector<Vector3> vertices;
	Projection result;
	float point;

	vertices.push_back(Vector3(object.transform.GetPosition() + (object.transform.GetRight() * object.transform.GetScale().x + object.transform.GetUp() * object.transform.GetScale().y + object.transform.GetForward() * object.transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object.transform.GetPosition() + (-object.transform.GetRight() * object.transform.GetScale().x + object.transform.GetUp() * object.transform.GetScale().y + object.transform.GetForward() * object.transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object.transform.GetPosition() + (-object.transform.GetRight() * object.transform.GetScale().x + object.transform.GetUp() * object.transform.GetScale().y - object.transform.GetForward() * object.transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object.transform.GetPosition() + (object.transform.GetRight() * object.transform.GetScale().x + object.transform.GetUp() * object.transform.GetScale().y - object.transform.GetForward() * object.transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object.transform.GetPosition() + (object.transform.GetRight() * object.transform.GetScale().x - object.transform.GetUp() * object.transform.GetScale().y + object.transform.GetForward() * object.transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object.transform.GetPosition() + (-object.transform.GetRight() * object.transform.GetScale().x - object.transform.GetUp() * object.transform.GetScale().y + object.transform.GetForward() * object.transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object.transform.GetPosition() + (-object.transform.GetRight() * object.transform.GetScale().x - object.transform.GetUp() * object.transform.GetScale().y - object.transform.GetForward() * object.transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object.transform.GetPosition() + (object.transform.GetRight() * object.transform.GetScale().x - object.transform.GetUp() * object.transform.GetScale().y - object.transform.GetForward() * object.transform.GetScale().z) / 2.0f));
	for (int i = 0; i < vertices.size(); i++)
	{
		point = Vector3::Dot(axis, vertices[i]);
		if (i == 0)
		{
			result.min = point;
			result.max = point;
			continue;
		}

		if (point < result.min)
			result.min = point;
		else if (point > result.max)
			result.max = point;
	}
	return result;
}
bool Collision::NarrowPhaseCubeAndCube(Object* object1, Object* object2, vector<Contact>& contacts, vector<Vector3>* lineForDebug)
{
	Contact contact;
	vector<Vector3> axes;

	// 첫번째 Cube의 Face Normal
	axes.push_back(object1->transform.GetRight());
	axes.push_back(object1->transform.GetUp());
	axes.push_back(object1->transform.GetForward());
	// 두번째 Cube의 Face Normal
	axes.push_back(object2->transform.GetRight());
	axes.push_back(object2->transform.GetUp());
	axes.push_back(object2->transform.GetForward());
	// 두 Cube의 Edge Cross
	axes.push_back(Vector3::Cross(object1->transform.GetRight(), object2->transform.GetRight()));
	axes.push_back(Vector3::Cross(object1->transform.GetRight(), object2->transform.GetUp()));
	axes.push_back(Vector3::Cross(object1->transform.GetRight(), object2->transform.GetForward()));
	axes.push_back(Vector3::Cross(object1->transform.GetUp(), object2->transform.GetRight()));
	axes.push_back(Vector3::Cross(object1->transform.GetUp(), object2->transform.GetUp()));
	axes.push_back(Vector3::Cross(object1->transform.GetUp(), object2->transform.GetForward()));
	axes.push_back(Vector3::Cross(object1->transform.GetForward(), object2->transform.GetRight()));
	axes.push_back(Vector3::Cross(object1->transform.GetForward(), object2->transform.GetUp()));
	axes.push_back(Vector3::Cross(object1->transform.GetForward(), object2->transform.GetForward()));

	Projection result1;
	Projection result2;
	float penetration;
	float minPenetration = 0;
	int minPenetrationAxisIndex=-1;
	for (int i = 0; i < axes.size(); i++)
	{
		if (axes[i] == Vector3::Zero())// Edge간 Cross 했을 때 평행한 경우는 제외
			continue;

		axes[i] = Vector3::Normalize(axes[i]);
		result1 = ProjectCubeToAxis(*object1, axes[i]);
		result2 = ProjectCubeToAxis(*object2, axes[i]);
		penetration = GetOverlappedAmount(result1, result2);

		if (penetration == 0)// 겹치지 않은 경우이므로 충돌 자체가 성립하지 않음
			return false;

		if (minPenetration == 0 || penetration < minPenetration)// 아직 minPenetration이 초기화 되지 않았거나 최근 결과가 더 작은 경우
		{
			minPenetration = penetration;
			minPenetrationAxisIndex = i;
		}
	}
	// 1차 검사 통과
	if (minPenetrationAxisIndex >= 0 && minPenetrationAxisIndex <= 2)
	{
		lineForDebug[0].push_back(object1->transform.GetPosition());
		lineForDebug[1].push_back(object2->transform.GetPosition());
		lineForDebug[2].push_back(Vector3(1, 0, 0));
		CubeAndVertex(*object2 , *object1, &contact, lineForDebug);
	}
	else if (minPenetrationAxisIndex >= 3 && minPenetrationAxisIndex <= 5)
	{
		lineForDebug[0].push_back(object1->transform.GetPosition());
		lineForDebug[1].push_back(object2->transform.GetPosition());
		lineForDebug[2].push_back(Vector3(0, 1, 0));
		CubeAndVertex(*object1, *object2, &contact, lineForDebug);
	}
	else if (minPenetrationAxisIndex >= 6 && minPenetrationAxisIndex <= 14)
	{
		lineForDebug[0].push_back(object1->transform.GetPosition());
		lineForDebug[1].push_back(object2->transform.GetPosition());
		lineForDebug[2].push_back(Vector3(0, 0, 1));
		CubeAndEdge(*object1, *object2, &contact, lineForDebug);
	}
	else
	{
		ErrorLogger::Log("Box의 SAT 검사가 15개 이상의 축을 검사함..");
	}
	for (int i = 0; i < 2; i++)
	{
		if (contact.m_objects[i]->rigidbody.IsKinematic())
			contact.m_objects[i] = nullptr;
	}
	contacts.push_back(contact);
	return true;
}
/*
void Collision::NarrowPhaseLayAndSphere(Vector3& layPosition, Vector3& layDirection, Object* object)
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
void Collision::NarrowPhaseSphereAndPlaneSpace(Object* object1, Object* object2, vector<Contact>& contacts)
{
	float distance = Vector3::Dot(object2->planeCollider.GetNormal(), object1->transform.GetPosition()) - object2->planeCollider.GetOffset() - object1->sphereCollider.GetRadius();
	// 점과 평면의 거리 공식 + 점이 아니라 구니까 radius도 고려한거임
	if (distance < 0)
	{
		Contact contact;
		contact.object1 = object1;
		contact.object2 = object2;
		contact.point = object1->transform.GetPosition() - object2->planeCollider.GetNormal() * (object1->sphereCollider.GetRadius() + distance);
		contact.normal = object2->planeCollider.GetNormal();
		contact.depth = distance;
		contacts.push_back(contact);
	}
}
void Collision::NarrowPhaseSphereAndPlane(Object* object1, Object* object2, vector<Contact>& contacts)
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
		contact.depth = centerDistance - object1->sphereCollider.GetRadius();
		// 평면이 양면일 수 있으니까 양면 다 쳌
		if (centerDistance < 0)
		{
			contact.normal *= -1;
			contact.depth *= -1;
		}
		contacts.push_back(contact);
	}
}
void Collision::NarrowPhaseCubeAndPlaneSpace(Object* object1, Object* object2, vector<Contact>& contacts)
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
			contact.depth = distance;
			contacts.push_back(contact);
		}
	}
}
*/

bool SAT(Object& object1, Object& object2, vector<Vector3>* lineForDebug)
{
	vector<Vector3> axes;
	
	// 1. Projection 축 뽑아내기
	{
		vector<Vector3> object1Edges;
		vector<Vector3> object2Edges;
		Vector3 edge[3];
		Vector3 vertex[3];
		Vector3 normal;
		// 첫 물체의 face normal
		for (int i = 0; i < object1.model.meshes.size(); i++)
		{
			Mesh mesh = object1.model.meshes[i];
			for (int j = 0; j < mesh.indices.size(); j += 3)
			{
				vertex[0] = object1.transform.GetWorldMatrix() * Vector3(mesh.vertices[mesh.indices[j]].pos);
				vertex[1] = object1.transform.GetWorldMatrix() * Vector3(mesh.vertices[mesh.indices[j+1]].pos);
				vertex[2] = object1.transform.GetWorldMatrix() * Vector3(mesh.vertices[mesh.indices[j+2]].pos);
				edge[0] = vertex[1] - vertex[0];
				edge[1] = vertex[2] - vertex[0];
				normal = Vector3::Normalize(Vector3::Cross(edge[0], edge[1]));
				axes.push_back(normal);
				edge[2] = vertex[2] - vertex[1];
				for (int k = 0; k < 3; k++)
				{
					object1Edges.push_back(edge[k]);
				}
				lineForDebug[0].push_back(vertex[0]);
				lineForDebug[1].push_back(vertex[1]);
				lineForDebug[0].push_back(vertex[1]);
				lineForDebug[1].push_back(vertex[2]);
				lineForDebug[0].push_back(vertex[2]);
				lineForDebug[1].push_back(vertex[0]);

			}
		}
		// 두번째 물체의 face normal
		for (int i = 0; i < object2.model.meshes.size(); i++)
		{
			Mesh mesh = object2.model.meshes[i];
			for (int j = 0; j < mesh.indices.size(); j += 3)
			{
				vertex[0] = object2.transform.GetWorldMatrix() * Vector3(mesh.vertices[mesh.indices[j]].pos);
				vertex[1] = object2.transform.GetWorldMatrix() * Vector3(mesh.vertices[mesh.indices[j + 1]].pos);
				vertex[2] = object2.transform.GetWorldMatrix() * Vector3(mesh.vertices[mesh.indices[j + 2]].pos);
				edge[0] = vertex[1] - vertex[0];
				edge[1] = vertex[2] - vertex[0];
				normal = Vector3::Normalize(Vector3::Cross(edge[0], edge[1]));
				axes.push_back(normal);
				edge[2] = vertex[2] - vertex[1];
				for (int k = 0; k < 3; k++)
				{
					object2Edges.push_back(edge[k]);
				}
				lineForDebug[0].push_back(vertex[0]);
				lineForDebug[1].push_back(vertex[1]);
				lineForDebug[0].push_back(vertex[1]);
				lineForDebug[1].push_back(vertex[2]);
				lineForDebug[0].push_back(vertex[2]);
				lineForDebug[1].push_back(vertex[0]);
			}
		}
		// 두 물체의 edge cross
		for (int i = 0; i < object1Edges.size(); i++)
		{
			for (int j = 0; j < object2Edges.size(); j++)
			{
				normal = Vector3::Cross(object1Edges[i], object2Edges[j]);
				if (normal == Vector3::Zero())
				{
					continue;
				}
				normal = Vector3::Normalize(normal);
				axes.push_back(normal);
				//HashMap<float, pair<float ,float>> 
				// 중복 제거, x를 키값으로서 해시 테이블 생성
				

			}
		}
	}

	// 2. Projection
	{
		Projection result1;
		Projection result2;
		for (int i = 0; i < axes.size(); i++)
		{
			result1 = ProjectVerticesToAxis(object1, axes[i]);
			result2 = ProjectVerticesToAxis(object2, axes[i]);
			float penetration = GetOverlappedAmount(result1, result2);
			// Proj p1 = projectToAxis(object1, axes[i]);
			// Proj p2 = projectToAxis(object2, axes[i]);
			// 
			// if(!p1.overlap(p2)){
			//     return false;
			// }
			// 각 물체의 모든 점을? 플젝 해서 
			if (penetration == 0)
				return false;

		}
	}
	//return true;
	return true;
}
bool Collision::SATTest(Object& object1, Object& object2, vector<Vector3>* lineForDebug)
{
	return SAT(object1, object2, lineForDebug);
}
