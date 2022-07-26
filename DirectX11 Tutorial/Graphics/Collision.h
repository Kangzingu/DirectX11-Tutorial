#pragma once
#include "Contact.h"
#include "vector"

class Collision
{
	static void SphereAndSphere(Transform& t1, Rigidbody& r1, SphereCollider& s1, Transform& t2, Rigidbody& r2, SphereCollider& s2, vector<Contact>& contacts);
};