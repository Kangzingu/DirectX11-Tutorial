#include "../Object.h"
#include "Collider.h"

void Collider::Initialize()
{
}
void Collider::Update(float deltaTime)
{
}
void Collider::SetEnabled(bool isEnabled)
{
	m_isEnabled = isEnabled;
}
void Collider::SetCenter(Vector3 center)
{
	m_center = center;
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
bool Collider::IsEnabled(){return m_isEnabled;}
Vector3 Collider::GetCenter(){return m_center;}
float SphereCollider::GetRadius(){return m_radius;}
Vector3 CubeCollider::GetSize(){return m_size;}
Vector3 PlaneCollider::GetNormal(){return m_normal;}
float PlaneCollider::GetOffset(){return m_offset;}