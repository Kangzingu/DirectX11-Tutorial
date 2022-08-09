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
	float penetration[3];
	float maxPenetration = 0;
	Vector3 maxPenetrationVertex;
	Vector3 maxPenetrationNormal;
	
	for (int i = 0; i < cubeCoordVertices.size(); i++)
	{
		penetration[0] = cubeHalfScale.x - abs(cubeCoordVertices[i].x);
		penetration[1] = cubeHalfScale.y - abs(cubeCoordVertices[i].y);
		penetration[2] = cubeHalfScale.z - abs(cubeCoordVertices[i].z);
		if (penetration[0] > 0 && penetration[1] > 0 && penetration[2] > 0)
		{
			float minPenetration = Vector3::Magnitude(object1.transform.GetScale()) + Vector3::Magnitude(object2.transform.GetScale());
			Vector3 minPenetrationVertex;
			Vector3 normal;
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
			if (minPenetration > maxPenetration && minPenetration != Vector3::Magnitude(object1.transform.GetScale()) + Vector3::Magnitude(object2.transform.GetScale()))
			{
				maxPenetration = minPenetration;
				maxPenetrationVertex = minPenetrationVertex;
				maxPenetrationNormal = normal;
			}
		}
	}

	/*lineForDebug[0].push_back(minPenetrationVertex);
	lineForDebug[1].push_back(minPenetrationVertex + (Vector3::Normalize(normal) * minPenetration / 2));
	lineForDebug[2].push_back(Vector3(1, 0, 0));*/
	contact->m_objects[0] = &object1;
	contact->m_objects[1] = &object2;
	contact->m_point = maxPenetrationVertex + (Vector3::Normalize(maxPenetrationNormal) * maxPenetration / 2);
	contact->m_normal = Vector3::Normalize(maxPenetrationNormal);
	contact->m_penetration = maxPenetration;
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

	float penetration;
	Vector3 contactPointOnCubeEdge;
	Vector3 contactPointOnEdge;

	float maxPenetration = 0;
	Vector3 maxPenetrationClosestPointOnCubeEdge;
	Vector3 maxPenetrationClosestPointOnEdge;
	for (int i = 0; i < cube1Edges.size(); i++)
	{
		float minPenetration = Vector3::Magnitude(object1.transform.GetScale()) + Vector3::Magnitude(object2.transform.GetScale());
		Vector3 closestPointOnCubeEdge;
		Vector3 closestPointOnEdge;
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
				// 이 경우 교차하지 않음
				continue;
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
		if (minPenetration > maxPenetration && minPenetration != Vector3::Magnitude(object1.transform.GetScale()) + Vector3::Magnitude(object2.transform.GetScale()))
		{
			maxPenetration = minPenetration;
			maxPenetrationClosestPointOnCubeEdge = contactPointOnCubeEdge;
			maxPenetrationClosestPointOnEdge = contactPointOnEdge;
		}
	}
	for (int i = 0; i < cube1Edges.size(); i++)
	{
		/*lineForDebug[0].push_back(cube1Edges[i].origin);
		lineForDebug[1].push_back(cube1Edges[i].origin + cube1Edges[i].direction * cube1Edges[i].length);
		lineForDebug[2].push_back(Vector3(0,0,1));*/
	}
	for (int i = 0; i < cube2Edges.size(); i++)
	{
		/*lineForDebug[0].push_back(cube2Edges[i].origin);
		lineForDebug[1].push_back(cube2Edges[i].origin + cube2Edges[i].direction * cube2Edges[i].length);
		lineForDebug[2].push_back(Vector3(0, 0, 1));*/
	}
	/*lineForDebug[0].push_back(contactPointOnEdge);
	lineForDebug[1].push_back(contactPointOnEdge + Vector3::Normalize(contactPointOnCubeEdge - contactPointOnEdge) * minPenetration / 2);
	lineForDebug[2].push_back(Vector3(0, 1, 0));*/
	contact->m_objects[0] = &object1;
	contact->m_objects[1] = &object2;
	contact->m_point = maxPenetrationClosestPointOnEdge + Vector3::Normalize(maxPenetrationClosestPointOnCubeEdge - maxPenetrationClosestPointOnEdge) * maxPenetration / 2;
	contact->m_normal = Vector3::Normalize(maxPenetrationClosestPointOnCubeEdge - maxPenetrationClosestPointOnEdge);
	contact->m_penetration = maxPenetration;
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
		CubeAndVertex(*object2 , *object1, &contact, lineForDebug);
		lineForDebug[0].push_back(object1->transform.GetPosition());
		lineForDebug[1].push_back(contact.m_point);
		lineForDebug[2].push_back(Vector3(1, 0, 0));
	}
	else if (minPenetrationAxisIndex >= 3 && minPenetrationAxisIndex <= 5)
	{
		lineForDebug[0].push_back(object1->transform.GetPosition());
		lineForDebug[1].push_back(contact.m_point);
		lineForDebug[2].push_back(Vector3(0, 1, 0));
		CubeAndVertex(*object1, *object2, &contact, lineForDebug);
	}
	else if (minPenetrationAxisIndex >= 6 && minPenetrationAxisIndex <= 14)
	{
		lineForDebug[0].push_back(object1->transform.GetPosition());
		lineForDebug[1].push_back(contact.m_point);
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
