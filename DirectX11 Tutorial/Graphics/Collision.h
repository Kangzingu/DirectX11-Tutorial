#pragma once
#include "Contact.h"
#include "vector"

class Collision
{
public:
	static bool BroadPhaseBoundingSphere(Object& object1, Object& object2, vector<Vector3>* lineForDebug);
	static bool BroadPhaseAxisAlignBoundingBox(Object& object1, Object& object2, vector<Vector3>* lineForDebug);

	static void NarrowPhaseSphereAndSphere(Object* object1, Object* object2, vector<Contact>& contacts);
	static void NarrowPhaseSphereAndCube(Object* object1, Object* object2, vector<Contact>& contacts);
	static bool NarrowPhaseCubeAndCube(Object* object1, Object* object2, vector<Contact>& contacts, vector<Vector3>* lineForDebug);
	
	static void NarrowPhaseLayAndSphere(Vector3& layPosition, Vector3& layDirection, Object* object);
	static void NarrowPhaseSphereAndPlaneSpace(Object* object1, Object* object2, vector<Contact>& contacts);
	static void NarrowPhaseSphereAndPlane(Object* object1, Object* object2, vector<Contact>& contacts);
	static void NarrowPhaseCubeAndPlaneSpace(Object* object1, Object* object2, vector<Contact>& contacts);

	static bool SATTest(Object& object1, Object& object2, vector<Vector3>* lineForDebug);
};