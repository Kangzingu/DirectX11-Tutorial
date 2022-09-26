#include "../Objects/Object.h"
#include "Collision.h"
/************************************/
/* 물체-1 이 물체-2 에 가서 박는것! */
/************************************/

pair<float, float> ProjectVerticesToAxis(Object& object, Vector3& axis)
{
	float point;
	pair<float, float> minmax;
	for (int i = 0; i < object.m_model.m_meshes.size(); i++)
	{
		Mesh mesh = object.m_model.m_meshes[i];
		for (int j = 0; j < mesh.m_vertices.size(); j++)
		{
			point = Vector3::Dot(axis, object.m_transform.GetWorldMatrix() * mesh.m_vertices[j].position);
			if (i == 0 && j == 0)
			{
				minmax.first = point;
				minmax.second = point;
				continue;
			}

			if (point < minmax.first)
				minmax.first = point;
			else if (point > minmax.second)
				minmax.second = point;
		}
	}
	return minmax;
}
pair<float, float> ProjectCubeToAxis(Object& object, Vector3& axis)
{
	vector<Vector3> vertices;
	pair<float, float> result;
	float point;

	vertices.push_back(Vector3(object.m_transform.GetPosition() + (object.m_transform.GetRight() * object.m_transform.GetScale().x + object.m_transform.GetUp() * object.m_transform.GetScale().y + object.m_transform.GetForward() * object.m_transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object.m_transform.GetPosition() + (-object.m_transform.GetRight() * object.m_transform.GetScale().x + object.m_transform.GetUp() * object.m_transform.GetScale().y + object.m_transform.GetForward() * object.m_transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object.m_transform.GetPosition() + (-object.m_transform.GetRight() * object.m_transform.GetScale().x + object.m_transform.GetUp() * object.m_transform.GetScale().y - object.m_transform.GetForward() * object.m_transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object.m_transform.GetPosition() + (object.m_transform.GetRight() * object.m_transform.GetScale().x + object.m_transform.GetUp() * object.m_transform.GetScale().y - object.m_transform.GetForward() * object.m_transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object.m_transform.GetPosition() + (object.m_transform.GetRight() * object.m_transform.GetScale().x - object.m_transform.GetUp() * object.m_transform.GetScale().y + object.m_transform.GetForward() * object.m_transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object.m_transform.GetPosition() + (-object.m_transform.GetRight() * object.m_transform.GetScale().x - object.m_transform.GetUp() * object.m_transform.GetScale().y + object.m_transform.GetForward() * object.m_transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object.m_transform.GetPosition() + (-object.m_transform.GetRight() * object.m_transform.GetScale().x - object.m_transform.GetUp() * object.m_transform.GetScale().y - object.m_transform.GetForward() * object.m_transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object.m_transform.GetPosition() + (object.m_transform.GetRight() * object.m_transform.GetScale().x - object.m_transform.GetUp() * object.m_transform.GetScale().y - object.m_transform.GetForward() * object.m_transform.GetScale().z) / 2.0f));
	for (int i = 0; i < vertices.size(); i++)
	{
		point = Vector3::Dot(axis, vertices[i]);
		if (i == 0)
		{
			result.first = point;
			result.second = point;
			continue;
		}

		if (point < result.first)
			result.first = point;
		else if (point > result.second)
			result.second = point;
	}
	return result;
}
float GetOverlappedAmount(pair<float, float> p1, pair<float, float> p2)
{
	//	p1	p2	p1	p2
	if (p2.first < p1.second && p2.second > p1.second)
		return p1.second - p2.first;

	//	p2	p1	p2	p1
	else if (p1.first < p2.second && p1.second > p2.second)
		return p2.second - p1.first;

	//	p1	p2	p2	p1
	else if (p1.first <= p2.first && p1.second >= p2.second)
		return min(p1.second - p2.first, p2.second - p1.first);

	//	p2	p1	p1	p2
	else if (p2.first <= p1.first && p2.second >= p1.second)
		return min(p2.second - p1.first, p1.second - p2.first);

	// 겹치지 않음
	return 0;
}
void CubeAndVertex(Object& object1, Object& object2, int axisIndexToSee, vector<Contact>& contacts, Contact* contact, vector<Vector3>* lineForDebug)
{
	vector<Vector3> vertices;
	vector<Vector3> cubeCoordVertices;

	vertices.push_back(Vector3(object1.m_transform.GetPosition() + (object1.m_transform.GetRight() * object1.m_transform.GetScale().x + object1.m_transform.GetUp() * object1.m_transform.GetScale().y + object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object1.m_transform.GetPosition() + (-object1.m_transform.GetRight() * object1.m_transform.GetScale().x + object1.m_transform.GetUp() * object1.m_transform.GetScale().y + object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object1.m_transform.GetPosition() + (-object1.m_transform.GetRight() * object1.m_transform.GetScale().x + object1.m_transform.GetUp() * object1.m_transform.GetScale().y - object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object1.m_transform.GetPosition() + (object1.m_transform.GetRight() * object1.m_transform.GetScale().x + object1.m_transform.GetUp() * object1.m_transform.GetScale().y - object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object1.m_transform.GetPosition() + (object1.m_transform.GetRight() * object1.m_transform.GetScale().x - object1.m_transform.GetUp() * object1.m_transform.GetScale().y + object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object1.m_transform.GetPosition() + (-object1.m_transform.GetRight() * object1.m_transform.GetScale().x - object1.m_transform.GetUp() * object1.m_transform.GetScale().y + object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object1.m_transform.GetPosition() + (-object1.m_transform.GetRight() * object1.m_transform.GetScale().x - object1.m_transform.GetUp() * object1.m_transform.GetScale().y - object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f));
	vertices.push_back(Vector3(object1.m_transform.GetPosition() + (object1.m_transform.GetRight() * object1.m_transform.GetScale().x - object1.m_transform.GetUp() * object1.m_transform.GetScale().y - object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f));

	for (int i = 0; i < vertices.size(); i++)
		cubeCoordVertices.push_back(object2.m_transform.GetRotationMatrix().Inverse() * object2.m_transform.GetTranslationMatrix().Inverse() * vertices[i]);

	float penetration[3];
	Vector3 cubeHalfScale = object2.m_transform.GetScale() / 2.0f;

	for (int i = 0; i < vertices.size(); i++)
	{
		penetration[0] = cubeHalfScale.x - abs(cubeCoordVertices[i].x);
		penetration[1] = cubeHalfScale.y - abs(cubeCoordVertices[i].y);
		penetration[2] = cubeHalfScale.z - abs(cubeCoordVertices[i].z);

		if (penetration[0] > 0 && penetration[1] > 0 && penetration[2] > 0)
		{
			contact->m_objects[0] = &object1;
			contact->m_objects[1] = &object2;
			contact->m_penetration = penetration[axisIndexToSee];
			switch (axisIndexToSee)
			{
			case 0:
				if (cubeCoordVertices[i].x >= 0)
				{
					contact->m_normal = object2.m_transform.GetRight();
				}
				else
				{
					contact->m_normal = -object2.m_transform.GetRight();
				}
				break;
			case 1:
				if (cubeCoordVertices[i].y >= 0)
				{
					contact->m_normal = object2.m_transform.GetUp();
				}
				else
				{
					contact->m_normal = -object2.m_transform.GetUp();
				}break;
			case 2:
				if (cubeCoordVertices[i].z >= 0)
				{
					contact->m_normal = object2.m_transform.GetForward();
				}
				else
				{
					contact->m_normal = -object2.m_transform.GetForward();
				}break;
			}
			contact->m_point = vertices[i] + (contact->m_normal * contact->m_penetration / 2);

			for (int i = 0; i < 2; i++)
			{
				if (contact->m_objects[i]->m_rigidbody.IsKinematic())
					contact->m_objects[i] = nullptr;
			}
			contacts.push_back(*contact);
			lineForDebug[0].push_back(contact->m_point);
			lineForDebug[1].push_back(vertices[i]);
			lineForDebug[2].push_back(Vector3(1, 0, 0));
		}
	}
	return;
}
void CubeAndEdge(Object& object1, Object& object2, float minPenetration, vector<Contact>& contacts, Contact* contact, vector<Vector3>* lineForDebug)
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
	vector<LineSegment> cube1Edges;
	cube1Edges.push_back(LineSegment(object1.m_transform.GetPosition() + (object1.m_transform.GetRight() * object1.m_transform.GetScale().x + object1.m_transform.GetUp() * object1.m_transform.GetScale().y + object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f, -object1.m_transform.GetRight(), object1.m_transform.GetScale().x));
	cube1Edges.push_back(LineSegment(object1.m_transform.GetPosition() + (-object1.m_transform.GetRight() * object1.m_transform.GetScale().x + object1.m_transform.GetUp() * object1.m_transform.GetScale().y + object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f, -object1.m_transform.GetForward(), object1.m_transform.GetScale().z));
	cube1Edges.push_back(LineSegment(object1.m_transform.GetPosition() + (-object1.m_transform.GetRight() * object1.m_transform.GetScale().x + object1.m_transform.GetUp() * object1.m_transform.GetScale().y - object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f, object1.m_transform.GetRight(), object1.m_transform.GetScale().x));
	cube1Edges.push_back(LineSegment(object1.m_transform.GetPosition() + (object1.m_transform.GetRight() * object1.m_transform.GetScale().x + object1.m_transform.GetUp() * object1.m_transform.GetScale().y - object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f, object1.m_transform.GetForward(), object1.m_transform.GetScale().z));
	cube1Edges.push_back(LineSegment(object1.m_transform.GetPosition() + (object1.m_transform.GetRight() * object1.m_transform.GetScale().x + object1.m_transform.GetUp() * object1.m_transform.GetScale().y + object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f, -object1.m_transform.GetUp(), object1.m_transform.GetScale().y));
	cube1Edges.push_back(LineSegment(object1.m_transform.GetPosition() + (-object1.m_transform.GetRight() * object1.m_transform.GetScale().x + object1.m_transform.GetUp() * object1.m_transform.GetScale().y + object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f, -object1.m_transform.GetUp(), object1.m_transform.GetScale().y));
	cube1Edges.push_back(LineSegment(object1.m_transform.GetPosition() + (-object1.m_transform.GetRight() * object1.m_transform.GetScale().x + object1.m_transform.GetUp() * object1.m_transform.GetScale().y - object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f, -object1.m_transform.GetUp(), object1.m_transform.GetScale().y));
	cube1Edges.push_back(LineSegment(object1.m_transform.GetPosition() + (object1.m_transform.GetRight() * object1.m_transform.GetScale().x + object1.m_transform.GetUp() * object1.m_transform.GetScale().y - object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f, -object1.m_transform.GetUp(), object1.m_transform.GetScale().y));
	cube1Edges.push_back(LineSegment(object1.m_transform.GetPosition() + (object1.m_transform.GetRight() * object1.m_transform.GetScale().x - object1.m_transform.GetUp() * object1.m_transform.GetScale().y + object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f, -object1.m_transform.GetRight(), object1.m_transform.GetScale().x));
	cube1Edges.push_back(LineSegment(object1.m_transform.GetPosition() + (-object1.m_transform.GetRight() * object1.m_transform.GetScale().x - object1.m_transform.GetUp() * object1.m_transform.GetScale().y + object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f, -object1.m_transform.GetForward(), object1.m_transform.GetScale().z));
	cube1Edges.push_back(LineSegment(object1.m_transform.GetPosition() + (-object1.m_transform.GetRight() * object1.m_transform.GetScale().x - object1.m_transform.GetUp() * object1.m_transform.GetScale().y - object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f, object1.m_transform.GetRight(), object1.m_transform.GetScale().x));
	cube1Edges.push_back(LineSegment(object1.m_transform.GetPosition() + (object1.m_transform.GetRight() * object1.m_transform.GetScale().x - object1.m_transform.GetUp() * object1.m_transform.GetScale().y - object1.m_transform.GetForward() * object1.m_transform.GetScale().z) / 2.0f, object1.m_transform.GetForward(), object1.m_transform.GetScale().z));

	vector<LineSegment> cube2Edges;
	cube2Edges.push_back(LineSegment(object2.m_transform.GetPosition() + (object2.m_transform.GetRight() * object2.m_transform.GetScale().x + object2.m_transform.GetUp() * object2.m_transform.GetScale().y + object2.m_transform.GetForward() * object2.m_transform.GetScale().z) / 2.0f, -object2.m_transform.GetRight(), object2.m_transform.GetScale().x));
	cube2Edges.push_back(LineSegment(object2.m_transform.GetPosition() + (-object2.m_transform.GetRight() * object2.m_transform.GetScale().x + object2.m_transform.GetUp() * object2.m_transform.GetScale().y + object2.m_transform.GetForward() * object2.m_transform.GetScale().z) / 2.0f, -object2.m_transform.GetForward(), object2.m_transform.GetScale().z));
	cube2Edges.push_back(LineSegment(object2.m_transform.GetPosition() + (-object2.m_transform.GetRight() * object2.m_transform.GetScale().x + object2.m_transform.GetUp() * object2.m_transform.GetScale().y - object2.m_transform.GetForward() * object2.m_transform.GetScale().z) / 2.0f, object2.m_transform.GetRight(), object2.m_transform.GetScale().x));
	cube2Edges.push_back(LineSegment(object2.m_transform.GetPosition() + (object2.m_transform.GetRight() * object2.m_transform.GetScale().x + object2.m_transform.GetUp() * object2.m_transform.GetScale().y - object2.m_transform.GetForward() * object2.m_transform.GetScale().z) / 2.0f, object2.m_transform.GetForward(), object2.m_transform.GetScale().z));
	cube2Edges.push_back(LineSegment(object2.m_transform.GetPosition() + (object2.m_transform.GetRight() * object2.m_transform.GetScale().x + object2.m_transform.GetUp() * object2.m_transform.GetScale().y + object2.m_transform.GetForward() * object2.m_transform.GetScale().z) / 2.0f, -object2.m_transform.GetUp(), object2.m_transform.GetScale().y));
	cube2Edges.push_back(LineSegment(object2.m_transform.GetPosition() + (-object2.m_transform.GetRight() * object2.m_transform.GetScale().x + object2.m_transform.GetUp() * object2.m_transform.GetScale().y + object2.m_transform.GetForward() * object2.m_transform.GetScale().z) / 2.0f, -object2.m_transform.GetUp(), object2.m_transform.GetScale().y));
	cube2Edges.push_back(LineSegment(object2.m_transform.GetPosition() + (-object2.m_transform.GetRight() * object2.m_transform.GetScale().x + object2.m_transform.GetUp() * object2.m_transform.GetScale().y - object2.m_transform.GetForward() * object2.m_transform.GetScale().z) / 2.0f, -object2.m_transform.GetUp(), object2.m_transform.GetScale().y));
	cube2Edges.push_back(LineSegment(object2.m_transform.GetPosition() + (object2.m_transform.GetRight() * object2.m_transform.GetScale().x + object2.m_transform.GetUp() * object2.m_transform.GetScale().y - object2.m_transform.GetForward() * object2.m_transform.GetScale().z) / 2.0f, -object2.m_transform.GetUp(), object2.m_transform.GetScale().y));
	cube2Edges.push_back(LineSegment(object2.m_transform.GetPosition() + (object2.m_transform.GetRight() * object2.m_transform.GetScale().x - object2.m_transform.GetUp() * object2.m_transform.GetScale().y + object2.m_transform.GetForward() * object2.m_transform.GetScale().z) / 2.0f, -object2.m_transform.GetRight(), object2.m_transform.GetScale().x));
	cube2Edges.push_back(LineSegment(object2.m_transform.GetPosition() + (-object2.m_transform.GetRight() * object2.m_transform.GetScale().x - object2.m_transform.GetUp() * object2.m_transform.GetScale().y + object2.m_transform.GetForward() * object2.m_transform.GetScale().z) / 2.0f, -object2.m_transform.GetForward(), object2.m_transform.GetScale().z));
	cube2Edges.push_back(LineSegment(object2.m_transform.GetPosition() + (-object2.m_transform.GetRight() * object2.m_transform.GetScale().x - object2.m_transform.GetUp() * object2.m_transform.GetScale().y - object2.m_transform.GetForward() * object2.m_transform.GetScale().z) / 2.0f, object2.m_transform.GetRight(), object2.m_transform.GetScale().x));
	cube2Edges.push_back(LineSegment(object2.m_transform.GetPosition() + (object2.m_transform.GetRight() * object2.m_transform.GetScale().x - object2.m_transform.GetUp() * object2.m_transform.GetScale().y - object2.m_transform.GetForward() * object2.m_transform.GetScale().z) / 2.0f, object2.m_transform.GetForward(), object2.m_transform.GetScale().z));

	float penetration;
	for (int i = 0; i < cube1Edges.size(); i++)
	{
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
				continue;

			float x = Vector3::Dot((o1 - o2), d1);
			float y = Vector3::Dot((o1 - o2), d2);
			float a = Vector3::Dot(d1, d1);
			float b = Vector3::Dot(d2, d2);
			float c = Vector3::Dot(d1, d2);
			s = (x * b - y * c) / (c * c - a * b);
			t = (x * c - y * a) / (c * c - a * b);
			if (s<0 || s>cube2Edges[j].length ||
				t<0 || t>cube1Edges[i].length)
				continue;

			closestPointOnCubeEdge = cube2Edges[j].origin + cube2Edges[j].direction * s;
			closestPointOnEdge = cube1Edges[i].origin + cube1Edges[i].direction * t;

			penetration  = Vector3::SquareMagnitude(closestPointOnCubeEdge - closestPointOnEdge);
			if (penetration == 0 || penetration > minPenetration)
				continue;
			penetration = sqrt(penetration);

			if (Vector3::SquareMagnitude(object2.m_transform.GetPosition() - closestPointOnCubeEdge) > Vector3::SquareMagnitude(object2.m_transform.GetPosition() - closestPointOnEdge) &&
				Vector3::SquareMagnitude(object1.m_transform.GetPosition() - closestPointOnEdge) > Vector3::SquareMagnitude(object1.m_transform.GetPosition() - closestPointOnCubeEdge))
			{

				contact->m_objects[0] = &object1;
				contact->m_objects[1] = &object2;
				contact->m_normal = Vector3::Normalize(closestPointOnCubeEdge - closestPointOnEdge);
				contact->m_penetration = penetration;
				contact->m_point = closestPointOnEdge + contact->m_normal * contact->m_penetration / 2;
				contacts.push_back(*contact);
				lineForDebug[0].push_back(closestPointOnCubeEdge);
				lineForDebug[1].push_back(closestPointOnEdge);
				lineForDebug[2].push_back(Vector3(0, 0, 1));
			}
		}
	}

}
bool Collision::BroadPhaseBoundingSphere(Object& object1, Object& object2)
{
	float object1Radius = object1.m_collider.GetBound().maxRadius;
	float object2Radius = object2.m_collider.GetBound().maxRadius;
	float centerDistance = Vector3::Magnitude(object1.m_transform.GetPosition() - object2.m_transform.GetPosition());
	if (centerDistance > object1Radius + object2Radius)
		return false;

	return true;
}
bool Collision::BroadPhaseAxisAlignBoundingBox(Object& object1, Object& object2)
{
	/* AABB는 계속 바뀌니까 그때그때 만들어 줘야 함
	* 그럼에도 불구하고 이걸로 한번 걸러주면 성능이 더 나아지긴 함
	* (FPS 기준 약 70 -> 170 정도로 개선)
	*/
	float maxLen = 0;
	float len;
	Vector3 object1MinPosition, object1MaxPosition;
	Vector3 object2MinPosition, object2MaxPosition;
	Vector3 object1VertexWorldPosition, object2VertexWorldPosition;
	object1MinPosition = object1.m_transform.GetWorldMatrix() * object1MinPosition;
	object1MaxPosition = object1.m_transform.GetWorldMatrix() * object1MaxPosition;
	for (int i = 0; i < object1.m_model.m_meshes.size(); i++)
	{
		for (UINT j = 0; j < object1.m_model.m_meshes[i].m_vertices.size(); j++)
		{
			object1VertexWorldPosition = object1.m_transform.GetWorldMatrix() * Vector3(object1.m_model.m_meshes[i].m_vertices[j].position.x,
																						object1.m_model.m_meshes[i].m_vertices[j].position.y,
																						object1.m_model.m_meshes[i].m_vertices[j].position.z);
			if (object1VertexWorldPosition.x < object1MinPosition.x)
				object1MinPosition.x = object1VertexWorldPosition.x;
			if (object1VertexWorldPosition.y < object1MinPosition.y)
				object1MinPosition.y = object1VertexWorldPosition.y;
			if (object1VertexWorldPosition.z < object1MinPosition.z)
				object1MinPosition.z = object1VertexWorldPosition.z;
			if (object1VertexWorldPosition.x > object1MaxPosition.x)
				object1MaxPosition.x = object1VertexWorldPosition.x;
			if (object1VertexWorldPosition.y > object1MaxPosition.y)
				object1MaxPosition.y = object1VertexWorldPosition.y;
			if (object1VertexWorldPosition.z > object1MaxPosition.z)
				object1MaxPosition.z = object1VertexWorldPosition.z;
		}
	}
	object2MinPosition = object2.m_transform.GetWorldMatrix() * object2MinPosition;
	object2MaxPosition = object2.m_transform.GetWorldMatrix() * object2MaxPosition;
	for (int i = 0; i < object2.m_model.m_meshes.size(); i++)
	{
		for (UINT j = 0; j < object2.m_model.m_meshes[i].m_vertices.size(); j++)
		{
			object2VertexWorldPosition = object2.m_transform.GetWorldMatrix() * Vector3(object2.m_model.m_meshes[i].m_vertices[j].position.x,
																						object2.m_model.m_meshes[i].m_vertices[j].position.y,
																						object2.m_model.m_meshes[i].m_vertices[j].position.z);
			if (object2VertexWorldPosition.x < object2MinPosition.x)
				object2MinPosition.x = object2VertexWorldPosition.x;
			if (object2VertexWorldPosition.y < object2MinPosition.y)
				object2MinPosition.y = object2VertexWorldPosition.y;
			if (object2VertexWorldPosition.z < object2MinPosition.z)
				object2MinPosition.z = object2VertexWorldPosition.z;
			if (object2VertexWorldPosition.x > object2MaxPosition.x)
				object2MaxPosition.x = object2VertexWorldPosition.x;
			if (object2VertexWorldPosition.y > object2MaxPosition.y)
				object2MaxPosition.y = object2VertexWorldPosition.y;
			if (object2VertexWorldPosition.z > object2MaxPosition.z)
				object2MaxPosition.z = object2VertexWorldPosition.z;
		}
	}
	if (object1MinPosition.x > object2MaxPosition.x || object1MaxPosition.x < object2MinPosition.x ||
		object1MinPosition.y > object2MaxPosition.y || object1MaxPosition.y < object2MinPosition.y ||
		object1MinPosition.z > object2MaxPosition.z || object1MaxPosition.z < object2MinPosition.z)
		return false;

	return true;
}
void Collision::NarrowPhaseSphereAndSphere(Object* object1, Object* object2, vector<Contact>& contacts)
{
	Vector3 centerDiff = object1->m_transform.GetPosition() - object2->m_transform.GetPosition();
	float centerDistance = Vector3::Magnitude(centerDiff);
	if (centerDistance < object1->m_sphereCollider.GetRadius() + object2->m_sphereCollider.GetRadius())
	{
		Contact contact;
		contact.m_objects[0] = object1;
		contact.m_objects[1] = object2;
		contact.m_normal = Vector3::Normalize(centerDiff);
		contact.m_penetration = object1->m_sphereCollider.GetRadius() + object2->m_sphereCollider.GetRadius() - centerDistance;
		contact.m_point = object2->m_transform.GetPosition() + contact.m_normal * (object2->m_sphereCollider.GetRadius() - contact.m_penetration / 2.0f);
		contacts.push_back(contact);
	}
}
void Collision::NarrowPhaseSphereAndCube(Object* object1, Object* object2, vector<Contact>& contacts)
{
	Vector3 cubeCoordSphereCenterPosition = object2->m_transform.GetRotationMatrix().Transpose() * object2->m_transform.GetTranslationMatrix().Inverse() * object1->m_transform.GetPosition();
	Vector3 cubeHalfScale = object2->m_transform.GetScale() / 2.0f;

	if (abs(cubeCoordSphereCenterPosition.x) - object1->m_sphereCollider.GetRadius() < cubeHalfScale.x &&
		abs(cubeCoordSphereCenterPosition.y) - object1->m_sphereCollider.GetRadius() < cubeHalfScale.y &&
		abs(cubeCoordSphereCenterPosition.z) - object1->m_sphereCollider.GetRadius() < cubeHalfScale.z)
	{
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

		if (Vector3::Magnitude(cubeCoordSphereCenterPosition - closestPoint) <= object1->m_sphereCollider.GetRadius())
		{
			Vector3 worldCoordClosestPoint = object2->m_transform.GetTranslationMatrix() * object2->m_transform.GetRotationMatrix() * closestPoint;
			Contact contact;
			contact.m_objects[0] = object1;
			contact.m_objects[1] = object2;
			contact.m_normal = Vector3::Normalize(object1->m_transform.GetPosition() - worldCoordClosestPoint);
			contact.m_penetration = object1->m_sphereCollider.GetRadius() - Vector3::Magnitude(object1->m_transform.GetPosition() - worldCoordClosestPoint);
			contact.m_point = worldCoordClosestPoint - contact.m_normal * contact.m_penetration / 2;
			contacts.push_back(contact);
		}
	}
}
void Collision::NarrowPhaseCubeAndCube(Object* object1, Object* object2, vector<Contact>& contacts, vector<Vector3>* lineForDebug)
{
	Contact contact;
	vector<Vector3> axes;
	axes.push_back(object1->m_transform.GetRight());
	axes.push_back(object1->m_transform.GetUp());
	axes.push_back(object1->m_transform.GetForward());
	axes.push_back(object2->m_transform.GetRight());
	axes.push_back(object2->m_transform.GetUp());
	axes.push_back(object2->m_transform.GetForward());
	axes.push_back(Vector3::Cross(object1->m_transform.GetRight(), object2->m_transform.GetRight()));
	axes.push_back(Vector3::Cross(object1->m_transform.GetRight(), object2->m_transform.GetUp()));
	axes.push_back(Vector3::Cross(object1->m_transform.GetRight(), object2->m_transform.GetForward()));
	axes.push_back(Vector3::Cross(object1->m_transform.GetUp(), object2->m_transform.GetRight()));
	axes.push_back(Vector3::Cross(object1->m_transform.GetUp(), object2->m_transform.GetUp()));
	axes.push_back(Vector3::Cross(object1->m_transform.GetUp(), object2->m_transform.GetForward()));
	axes.push_back(Vector3::Cross(object1->m_transform.GetForward(), object2->m_transform.GetRight()));
	axes.push_back(Vector3::Cross(object1->m_transform.GetForward(), object2->m_transform.GetUp()));
	axes.push_back(Vector3::Cross(object1->m_transform.GetForward(), object2->m_transform.GetForward()));

	pair<float, float> result1;
	pair<float, float> result2;
	float penetration;
	float minPenetration = 0;
	int minPenetrationAxisIndex = -1;

	// 겹치긴 하는데 가장 적게 겹친 축을 찾음(해결하기 위해선 조금만 밀어주는게 자연스러울 거니까..?)
	for (int i = 0; i < axes.size(); i++)
	{
		if (axes[i] == Vector3::Zero())// Edge간 Cross 했을 때 평행한 경우는 제외
			continue;

		axes[i] = Vector3::Normalize(axes[i]);
		result1 = ProjectCubeToAxis(*object1, axes[i]);
		result2 = ProjectCubeToAxis(*object2, axes[i]);
		penetration = GetOverlappedAmount(result1, result2);

		if (penetration == 0)// 겹치지 않은 경우이므로 충돌 자체가 성립하지 않음
			return;

		if (minPenetration == 0 || penetration < minPenetration)// 아직 minPenetration이 초기화 되지 않았거나 최근 결과가 더 작은 경우
		{
			minPenetration = penetration;
			minPenetrationAxisIndex = i;
		}
	}

	if (minPenetrationAxisIndex >= 0 && minPenetrationAxisIndex <= 2)
	{
		CubeAndVertex(*object2, *object1, minPenetrationAxisIndex, contacts, &contact, lineForDebug);
	}
	else if (minPenetrationAxisIndex >= 3 && minPenetrationAxisIndex <= 5)
	{
		CubeAndVertex(*object1, *object2, minPenetrationAxisIndex-3, contacts, &contact, lineForDebug);
	}
	else if (minPenetrationAxisIndex >= 6 && minPenetrationAxisIndex <= 14)
	{
		CubeAndEdge(*object1, *object2, minPenetration, contacts, &contact, lineForDebug);
	}
	else
	{
		ErrorLogger::Log("Box의 SAT 검사가 15개 이상의 축을 검사함..");
	}
}
void Collision::NarrowPhaseConvexAndConvex(Object& object1, Object& object2, vector<Contact>& contacts, vector<Vector3>* lineForDebug)
{
	Contact contact;
	vector<Vector3> axes;

	vector<Vector3> faceCenterPositions;
	int object1FaceCount = 0;
	int object2FaceCount = 0;

	vector<LineSegment> object1Edges;
	vector<LineSegment> object2Edges;

	// 1. Projection 축 뽑아내기
	{
		LineSegment edge[3];
		Vector3 vertex[3];
		Vector3 normal;
		// 첫 물체의 face normal
		for (int i = 0; i < object1.m_model.m_meshes.size(); i++)
		{
			Mesh mesh = object1.m_model.m_meshes[i];
			for (int j = 0; j < mesh.m_indices.size(); j += 3)
			{
				vertex[0] = object1.m_transform.GetWorldMatrix() * Vector3(mesh.m_vertices[mesh.m_indices[j]].position);
				vertex[1] = object1.m_transform.GetWorldMatrix() * Vector3(mesh.m_vertices[mesh.m_indices[j + 1]].position);
				vertex[2] = object1.m_transform.GetWorldMatrix() * Vector3(mesh.m_vertices[mesh.m_indices[j + 2]].position);

				edge[0].direction = Vector3::Normalize(vertex[1] - vertex[0]);
				edge[0].length = Vector3::Magnitude(vertex[1] - vertex[0]);
				edge[0].origin = vertex[0];
				edge[1].direction = Vector3::Normalize(vertex[2] - vertex[0]);
				edge[1].length = Vector3::Magnitude(vertex[2] - vertex[0]);
				edge[1].origin = vertex[0];
				edge[2].direction = Vector3::Normalize(vertex[2] - vertex[1]);
				edge[2].length = Vector3::Magnitude(vertex[2] - vertex[1]);
				edge[2].origin = vertex[1];

				axes.push_back(Vector3::Cross(edge[0].direction, edge[1].direction));
				faceCenterPositions.push_back((vertex[0] + vertex[1] + vertex[2]) / 3);
				object1FaceCount++;

				for (int k = 0; k < 3; k++)
				{
					object1Edges.push_back(edge[k]);
				}
				/*lineForDebug[0].push_back(vertex[0]);
				lineForDebug[1].push_back(vertex[1]);
				lineForDebug[0].push_back(vertex[1]);
				lineForDebug[1].push_back(vertex[2]);
				lineForDebug[0].push_back(vertex[2]);
				lineForDebug[1].push_back(vertex[0]);*/
			}
		}
		// 두번째 물체의 face normal
		for (int i = 0; i < object2.m_model.m_meshes.size(); i++)
		{
			Mesh mesh = object2.m_model.m_meshes[i];
			for (int j = 0; j < mesh.m_indices.size(); j += 3)
			{
				vertex[0] = object2.m_transform.GetWorldMatrix() * Vector3(mesh.m_vertices[mesh.m_indices[j]].position);
				vertex[1] = object2.m_transform.GetWorldMatrix() * Vector3(mesh.m_vertices[mesh.m_indices[j + 1]].position);
				vertex[2] = object2.m_transform.GetWorldMatrix() * Vector3(mesh.m_vertices[mesh.m_indices[j + 2]].position);

				edge[0].direction = Vector3::Normalize(vertex[1] - vertex[0]);
				edge[0].length = Vector3::Magnitude(vertex[1] - vertex[0]);
				edge[0].origin = vertex[0];
				edge[1].direction = Vector3::Normalize(vertex[2] - vertex[0]);
				edge[1].length = Vector3::Magnitude(vertex[2] - vertex[0]);
				edge[1].origin = vertex[0];
				edge[2].direction = Vector3::Normalize(vertex[2] - vertex[1]);
				edge[2].length = Vector3::Magnitude(vertex[2] - vertex[1]);
				edge[2].origin = vertex[1];

				axes.push_back(Vector3::Cross(edge[0].direction, edge[1].direction));
				faceCenterPositions.push_back((vertex[0] + vertex[1] + vertex[2]) / 3);
				object2FaceCount++;

				for (int k = 0; k < 3; k++)
				{
					object2Edges.push_back(edge[k]);
				}
			}
		}
		// 두 물체의 edge cross
		for (int i = 0; i < object1Edges.size(); i++)
		{
			for (int j = 0; j < object2Edges.size(); j++)
			{
				normal = Vector3::Cross(object1Edges[i].direction, object2Edges[j].direction);
				axes.push_back(normal);
				//HashMap<float, pair<float ,float>> 
				// 중복 제거, x를 키값으로서 해시 테이블 생성
			}
		}
	}

	// 2. Projection
	{
		pair<float, float> result1;
		pair<float, float> result2;
		float minPenetration = 0;
		int minPenetrationAxisIndex = -1;
		for (int i = 0; i < axes.size(); i++)
		{
			if (axes[i] == Vector3::Zero())
				continue;

			axes[i] = Vector3::Normalize(axes[i]);
			result1 = ProjectVerticesToAxis(object1, axes[i]);
			result2 = ProjectVerticesToAxis(object2, axes[i]);
			float penetration = GetOverlappedAmount(result1, result2);

			if (penetration == 0)
				return;

			if (minPenetration == 0 ||
				penetration < minPenetration)
			{
				minPenetration = penetration;
				minPenetrationAxisIndex = i;
			}
		}

		if (minPenetrationAxisIndex >= 0 && minPenetrationAxisIndex <= object1FaceCount - 1)
		{
			Vector3 contactPointOnFace = object2.m_transform.GetPosition() - axes[minPenetrationAxisIndex] * Vector3::Dot(axes[minPenetrationAxisIndex], object2.m_transform.GetPosition() - faceCenterPositions[minPenetrationAxisIndex]);
			contact.m_objects[0] = &object2;
			contact.m_objects[1] = &object1;
			contact.m_normal = axes[minPenetrationAxisIndex];
			contact.m_penetration = minPenetration;
			contact.m_point = contactPointOnFace - (contact.m_normal * contact.m_penetration / 2);

		}
		else if (minPenetrationAxisIndex >= object1FaceCount && minPenetrationAxisIndex <= object1FaceCount + object2FaceCount - 1)
		{
			Vector3 contactPointOnFace = object2.m_transform.GetPosition() - axes[minPenetrationAxisIndex] * Vector3::Dot(axes[minPenetrationAxisIndex], object2.m_transform.GetPosition() - faceCenterPositions[minPenetrationAxisIndex]);
			contact.m_objects[0] = &object1;
			contact.m_objects[1] = &object2;
			contact.m_normal = axes[minPenetrationAxisIndex];
			contact.m_penetration = minPenetration;
			contact.m_point = contactPointOnFace - (contact.m_normal * contact.m_penetration / 2);

		}
		else if (minPenetrationAxisIndex >= object1FaceCount + object2FaceCount && minPenetrationAxisIndex <= axes.size() - 1)
		{
			int i = (minPenetrationAxisIndex - (object1FaceCount + object2FaceCount)) / object2Edges.size();
			int j = (minPenetrationAxisIndex - (object1FaceCount + object2FaceCount)) % object2Edges.size();
			Vector3 o1 = object2Edges[j].origin;
			Vector3 o2 = object1Edges[i].origin;
			Vector3 d1 = object2Edges[j].direction;
			Vector3 d2 = object1Edges[i].direction;
			float s, t;
			if (Vector3::Cross(d1, d2) == Vector3::Zero())
				return;

			float x = Vector3::Dot((o1 - o2), d1);
			float y = Vector3::Dot((o1 - o2), d2);
			float a = Vector3::Dot(d1, d1);
			float b = Vector3::Dot(d2, d2);
			float c = Vector3::Dot(d1, d2);
			s = (x * b - y * c) / (c * c - a * b);
			t = (x * c - y * a) / (c * c - a * b);
			if (s<0 || s>object2Edges[j].length ||
				t<0 || t>object1Edges[i].length)
				return;

			Vector3 closestPointOnCubeEdge = object2Edges[j].origin + object2Edges[j].direction * s;
			Vector3 closestPointOnEdge = object1Edges[i].origin + object1Edges[i].direction * t;

			contact.m_objects[0] = &object1;
			contact.m_objects[1] = &object2;
			contact.m_normal = Vector3::Normalize(closestPointOnCubeEdge - closestPointOnEdge);
			contact.m_penetration = minPenetration;
			contact.m_point = closestPointOnEdge + contact.m_normal * contact.m_penetration / 2;

			//return false;
		}
		else
		{
			ErrorLogger::Log("Convex의 SAT 검사가 범위 이상의 축을 검사함..");
		}

		//return true;
		for (int i = 0; i < 2; i++)
		{
			if (contact.m_objects[i]->m_rigidbody.IsKinematic())
				contact.m_objects[i] = nullptr;
		}
		lineForDebug[0].push_back(contact.m_point);
		lineForDebug[1].push_back(contact.m_point + contact.m_normal * contact.m_penetration);
		lineForDebug[2].push_back(Vector3(0, 0, 1));
		contacts.push_back(contact);
	}
}


/************************************/
/* GJK Algorithm                    */
/************************************/
Vector3 GJKSupport(Object& object1, Object& object2, Vector3 direction)
{
	if (direction == Vector3::Zero())
		assert("두 물체의 위치가 동일해서 초기 direction 값이 0임");
	 Vector3 object1MaxDistanceVertex = object1.m_model.GetFarthestVertexOfDirectionWorldCoord(direction);
	 Vector3 object2MaxDistanceVertex = object2.m_model.GetFarthestVertexOfDirectionWorldCoord(-direction);
	
	return object1MaxDistanceVertex - object2MaxDistanceVertex;
}
bool GJKDoesSimplexContainsOrigin(vector<Vector3>& simplex)
{
	Vector3 lastAddedPoint = simplex[simplex.size() - 1];
	return false;
}
bool GJK(Object& object1, Object& object2)
{
	vector<Vector3> simplex;
	Vector3 direction = object2.m_transform.GetPosition() - object1.m_transform.GetPosition();

	simplex.push_back(GJKSupport(object1, object2, direction));

	direction *= -1;

	while (true)
	{
		simplex.push_back(GJKSupport(object1, object2, direction));
		if (Vector3::Dot(simplex.back(), direction) < 0)
		{
			assert("두 물체는 충돌하지 않음");
			return false;
		}
		else
		{
			if (GJKDoesSimplexContainsOrigin(simplex) == true)
				return true;
		}
	}
}