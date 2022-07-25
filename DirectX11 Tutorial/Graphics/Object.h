#pragma once
#include "Transform.h"
#include "Rigidbody.h"
#include "Collider.h"

class Object
{
public:
	Object() {}
	void Initialize(Model& model, Transform& transform, Rigidbody& rigidbody, Collider& collider);
	virtual void Draw(Matrix4x4& viewProjectionMatrix) {};
	
	Model model;
	Transform transform;
	Rigidbody rigidbody;
	Collider collider;
};