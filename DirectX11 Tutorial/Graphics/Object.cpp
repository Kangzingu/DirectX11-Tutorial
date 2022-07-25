#include "Object.h"

void Object::Initialize(Model& model, Transform& transform, Rigidbody& rigidbody, Collider& collider)
{
	this->model = model;
	this->transform = transform;
	this->rigidbody = rigidbody;
	this->collider = collider;
}