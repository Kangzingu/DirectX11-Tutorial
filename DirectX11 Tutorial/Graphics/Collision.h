#pragma once
#include "Contact.h"
#include "vector"

class Collision
{
	static void SphereAndSphere(Transform& t1, Rigidbody& r1, SphereCollider& c1, Transform& t2, Rigidbody& r2, SphereCollider& c2, vector<Contact>& contacts);
	static void SphereAndPlaneSpace(Transform& t1, Rigidbody& r1, SphereCollider& c1, Transform& t2, Rigidbody& r2, PlaneCollider& c2, vector<Contact>& contacts);
	static void SphereAndPlane(Transform& t1, Rigidbody& r1, SphereCollider& c1, Transform& t2, Rigidbody& r2, PlaneCollider& c2, vector<Contact>& contacts);
	static void SphereAndCube(Transform& t1, Rigidbody& r1, SphereCollider& c1, Transform& t2, Rigidbody& r2, CubeCollider& c2, vector<Contact>& contacts);
	static void CubeAndPlaneSpace(Transform& t1, Rigidbody& r1, CubeCollider& c1, Transform& t2, Rigidbody& r2, PlaneCollider& c2, vector<Contact>& contacts);
};