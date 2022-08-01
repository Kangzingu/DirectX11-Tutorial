#pragma once
#include "Contact.h"
#include "vector"

class Collision
{
public:
	static bool BroadPhaseBoundingSphere(Object& object1, Object& object2, vector<pair<Vector3, Vector3>>& aabb);
	static bool BroadPhaseAxisAlignBoundingBox(Object& object1, Object& object2, vector<pair<Vector3, Vector3>>& aabb);

	static void SphereAndSphere(Object* object1, Object* object2, vector<Contact>& contacts);
	static void SphereAndCube(Object* object1, Object* object2, vector<Contact>& contacts);
	static bool CubeAndCube(Object* object1, Object* object2, vector<Contact>& contacts);
	
	static void LayAndSphere(Vector3& layPosition, Vector3& layDirection, Object* object);
	static void SphereAndPlaneSpace(Object* object1, Object* object2, vector<Contact>& contacts);
	static void SphereAndPlane(Object* object1, Object* object2, vector<Contact>& contacts);
	static void CubeAndPlaneSpace(Object* object1, Object* object2, vector<Contact>& contacts);
};