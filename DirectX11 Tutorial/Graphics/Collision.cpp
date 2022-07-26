#include "Collision.h"
/************************************/
/* ��ü-1 �� ��ü-2 �� �ͼ� �ڴ°�! */
/************************************/

void Collision::SphereAndSphere(Transform& t1, Rigidbody& r1, SphereCollider& s1,Transform& t2, Rigidbody& r2, SphereCollider& s2, vector<Contact>& contacts)
{
	Vector3 centerPositionDiff= t1.GetPosition() - t2.GetPosition();
	float centerPositionDistance = Vector3::Magnitude(centerPositionDiff);
	if (centerPositionDistance < s1.radius + s2.radius)
	{
		Contact contact;
		contact.t1 = &t1;
		contact.t2 = &t2;
		contact.point = t2.GetPosition() + (centerPositionDiff / 2.0f);
		contact.normal = -Vector3::Normalize(centerPositionDiff);
		contact.penetration = s1.radius + s2.radius - centerPositionDistance;
	}
}