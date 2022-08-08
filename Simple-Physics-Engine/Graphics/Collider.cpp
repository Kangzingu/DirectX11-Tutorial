#include "Object.h"
#include "Collider.h"

void Collider::Initialize()
{
}
void Collider::Update(float deltaTime)
{
}
void Collider::SetEnabled(bool isEnabled)
{
	this->isEnabled = isEnabled;
}
void Collider::SetCenter(Vector3 center)
{
	this->center = center;
}
void SphereCollider::SetRadius(float radius)
{
	this->radius = radius;
}
void CubeCollider::SetSize(Vector3 size)
{
	this->size = size;
}
void PlaneCollider::SetNormal(Vector3 normal)
{
	this->normal = normal;
}
void PlaneCollider::SetOffset(float offset)
{
	this->offset = offset;
}
bool Collider::IsEnabled(){return isEnabled;}
Vector3 Collider::GetCenter(){return center;}
float SphereCollider::GetRadius(){return radius;}
Vector3 CubeCollider::GetSize(){return size;}
Vector3 PlaneCollider::GetNormal(){return normal;}
float PlaneCollider::GetOffset(){return offset;}