#include "Object.h"

void Object::Initialize(Model& model, Transform& transform, Rigidbody& rigidbody, Collider& collider)
{
	m_model = model;
	m_transform = transform;
	m_rigidbody = rigidbody;
	m_collider = collider;

	m_model.m_object = this;
	m_transform.m_object = this;
	m_rigidbody.m_object = this;
	m_collider.m_object = this;
}