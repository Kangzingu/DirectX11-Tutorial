#include "../../Object.h"
#include "Collider.h"

void Collider::Initialize()
{
}
void Collider::Update(float deltaTime)
{
}
void Collider::SetCenter(Vector3 center)
{
	m_center = center;
}
void Collider::SetBound(Bound bound)
{
	m_bound = bound;
}
void Collider::UpdateBound()
{
	float maxVertexDistance = 0;
	float minVertexDistance = FLT_MAX;
	float vertexDistance;
	for (int i = 0; i < m_object->m_model.m_meshes.size(); i++)
	{
		for (int j = 0; j < m_object->m_model.m_meshes[i].m_vertices.size(); j++)
		{
			vertexDistance = Vector3::SquareMagnitude(Vector3(m_object->m_model.m_meshes[i].m_vertices[j].position.x * m_object->m_transform.GetScale().x,
														m_object->m_model.m_meshes[i].m_vertices[j].position.y * m_object->m_transform.GetScale().y,
														m_object->m_model.m_meshes[i].m_vertices[j].position.z * m_object->m_transform.GetScale().z));
			if (vertexDistance < minVertexDistance)
				minVertexDistance = vertexDistance;
			if (vertexDistance > maxVertexDistance)
				maxVertexDistance = vertexDistance;
		}
	}
	m_bound.minRadius = sqrt(minVertexDistance);
	m_bound.maxRadius = sqrt(maxVertexDistance);
}
void SphereCollider::SetRadius(float radius)
{
	m_radius = radius;
}
void CubeCollider::SetSize(Vector3 size)
{
	m_size = size;
}
void PlaneCollider::SetNormal(Vector3 normal)
{
	m_normal = normal;
}
void PlaneCollider::SetOffset(float offset)
{
	m_offset = offset;
}
Vector3 Collider::GetCenter(){return m_center;}
Bound Collider::GetBound(){return m_bound;}
float SphereCollider::GetRadius(){return m_radius;}
Vector3 CubeCollider::GetSize(){return m_size;}
Vector3 PlaneCollider::GetNormal(){return m_normal;}
float PlaneCollider::GetOffset(){return m_offset;}