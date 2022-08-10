#pragma once
#include "Contact.h"
#include "vector"

class Collision
{
public:
	static bool BroadPhaseBoundingSphere(Object& object1, Object& object2);
	static bool BroadPhaseAxisAlignBoundingBox(Object& object1, Object& object2);

	static void NarrowPhaseSphereAndSphere(Object* object1, Object* object2, vector<Contact>& contacts);
	static void NarrowPhaseSphereAndCube(Object* object1, Object* object2, vector<Contact>& contacts);
	static bool NarrowPhaseCubeAndCube(Object* object1, Object* object2, vector<Contact>& contacts, vector<Vector3>* lineForDebug);
	static bool NarrowPhaseConvexAndConvex(Object& object1, Object& object2, vector<Vector3>* lineForDebug);
};