#include "ForceGenerator.h"

void ForceGenerator::GenerateGravity(Rigidbody& rigidbody)
{
	rigidbody.AddForce(gravity.force * rigidbody.mass);
}

// �и� �ӵ� = (a�� �ӵ� - b�� �ӵ�) * (b���� a�� ���ϴ� ����)
// a�� ���� * a�� �ӵ� + b�� ���� * b�� �ӵ� = a�� �浹 ���� ���� * a�� �浹 ���� �ӵ� + b�� �浹 ���� ���� * b�� �浹 ���� �ӵ�
// �浹 ���� �и� �ӵ� = -�ݹ߰�� * �и� �ӵ�

//// �浹
//float contactDepth = XMVectorGetX(XMVector3Length(actors[0].transform.GetPositionVector() - actors[1].transform.GetPositionVector())) - 30.0f;
//if (contactDepth < 0.0f)
//{
//	actors[0].rigidbody.mass = 100000.0f;

//	contact.restitution = 0.4f;
//	contact.contactNormal = XMVector3Normalize(actors[1].transform.GetPositionVector() - actors[0].transform.GetPositionVector());
//	contact.penetration = -contactDepth;
//	contact.actors[0] = &actors[1];
//	contact.actors[1] = &actors[0];
//	contact.Resolve(deltaTime);
//}

//// �η�
//for (int i = 0; i < actors.size(); i++)
//{
//	if (i == 0) continue;
//	float d = actors[i].transform.GetPositionFloat3().y;
//	float waterHeight = actors[0].transform.GetPositionFloat3().y;
//	float maxDepth = 0.5f;
//	float volumn = 1.0f;// ���� ����.. m^3 ������
//	float density = 1000.0f;// ���� �е�.. kg/m^3 ������
//	XMVECTOR bouyancy;
//	// y=0.5	d=0
//	// y=0		d=0.5
//	// y=-0.5	d=1
//	if (d >= waterHeight + maxDepth)
//	{
//		bouyancy = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
//	}
//	else if (d <= waterHeight - maxDepth)
//	{
//		bouyancy = XMVectorSet(0.0f, volumn * density, 0.0f, 0.0f);
//	}
//	else
//	{
//		bouyancy = XMVectorSet(0.0f, volumn * density * (d - maxDepth - waterHeight) / (2 * maxDepth), 0.0f, 0.0f);
//	}
//	actors[i].rigidbody.AddForce(bouyancy);
//}

//// ������ ��(����)
//for (int i = 1; i < actors.size(); i++)
//{
//	XMVECTOR spring = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
//	float k = 30.0f;
//	float distAtFirst = sqrt(25.0f);
//	if (XMVectorGetX(XMVector3Length(actors[i].GetPositionVector() - actors[0].GetPositionVector())) < distAtFirst) continue;
//	spring += -k * XMVector3Normalize(actors[i].GetPositionVector() - actors[0].GetPositionVector()) * (XMVectorGetX(XMVector3Length(actors[i].GetPositionVector() - actors[0].GetPositionVector())) - distAtFirst);
//	actors[i].rigidbody.AddForce(spring);
//}

//// ������ ��
//for (int i = 0; i < 1; i++)
//{
//	XMVECTOR connectedLocalPoint = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
//	XMVECTOR otherConnectedLocalPoint = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
//	XMVECTOR connectedWorldPoint = XMVector3Transform(connectedLocalPoint, actors[0].transform.worldMatrix);
//	XMVECTOR otherConnectedWorldPoint = XMVector3Transform(otherConnectedLocalPoint, actors[1].transform.worldMatrix);
//	XMVECTOR force = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
//	float k = 30.0f;
//	float distAtFirst = sqrt(25.0f);
//	force += k * XMVector3Normalize(connectedWorldPoint - otherConnectedWorldPoint) * (XMVectorGetX(XMVector3Length(connectedWorldPoint - otherConnectedWorldPoint)) - distAtFirst);
//	actors[1].rigidbody.AddForceAt(force, otherConnectedWorldPoint);
//}

//// �׷�
//for (int i = 0; i < actors.size(); i++)
//{
//	XMVECTOR drag;
//	float k1 = 10.0f, k2 = 10.0f;
//	// ���� �ٸ��� �ѵ� ��ü�� ������� ���� �������װ���� ������ ����
//	// ��: 0.47
//	// ť��: 1.05
//	// �Ǹ���: 0.82
//	float velocityMagnitude = XMVectorGetX(XMVector3Length(actors[i].rigidbody.velocity));
//	drag = -XMVector3Normalize(actors[i].rigidbody.velocity) * (k1 * velocityMagnitude + k2 * velocityMagnitude * velocityMagnitude);
//	actors[i].rigidbody.AddForce(drag);
//}

/*static bool isDirUp = false;
actors[0].rigidbody.accumulatedForce = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
if (actors[0].transform.GetPositionFloat3().y > 1.0f)
	isDirUp = false;
else if (actors[0].transform.GetPositionFloat3().y < -1.0f)
	isDirUp = true;
if (isDirUp == true)
	actors[0].transform.AdjustPosition(0, 0.0002f, 0);
else
	actors[0].transform.AdjustPosition(0, -0.0002f, 0);*/