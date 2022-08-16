#pragma once
#include "../Components.h"
#include "../Utils/SimpleMath.h"

struct Bound
{
	float minRadius = 0;
	float maxRadius = 0;
};
class Collider : public Components
{
private:
	Vector3 m_center = Vector3::Zero();
	Bound m_bound;

public:
	void Initialize();
	void Update(float deltaTime);
	void SetCenter(Vector3 center);
	void SetBound(Bound bound);
	void UpdateBound();

	Vector3& GetCenter();
	Bound& GetBound();
};

class SphereCollider : public Collider
{
private:
	float m_radius = 0.5f;
	
public:
	void SetRadius(float radius);
	float GetRadius();
};

class CubeCollider : public Collider
{
private:
	Vector3 m_size = Vector3::One();

public:
	void SetSize(Vector3 size);
	Vector3 GetSize();
};

class MeshCollider : public Collider
{

};

class PlaneCollider : public Collider
{
private:
	float m_offset = 0;
	Vector3 m_normal = Vector3::Forward();

public:
	void SetOffset(float offset);
	void SetNormal(Vector3 normal);
	float GetOffset();
	Vector3 GetNormal();
};