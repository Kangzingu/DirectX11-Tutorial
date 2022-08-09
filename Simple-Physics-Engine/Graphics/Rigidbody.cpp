#include "Object.h"
#include "Rigidbody.h"

void Rigidbody::Initialize(float inverseMass, float damping, float angularDamping, Vector3 velocity, Vector3 rotationVelocity, Matrix4x4 inertiaTensor)
{
	this->inverseMass = inverseMass;
	this->damping = damping;
	this->angularDamping = angularDamping;
	this->velocity = velocity;
	this->angularVelocity = rotationVelocity;
	this->inertiaTensor = inertiaTensor;
	this->ClearAccumulatedForce();
}
void Rigidbody::AddForce(Vector3 force)
{
	accumulatedForce += force;
}
void Rigidbody::AddForceAt(Vector3 force, Vector3 worldPoint)
{
	accumulatedForce += force;
	accumulatedTorque +=  (Vector3::Cross(worldPoint - object->transform.GetPosition(), force));
}
void Rigidbody::AddTorqueAt(Vector3 force, Vector3 worldPoint)
{
	accumulatedTorque += (Vector3::Cross(worldPoint - object->transform.GetPosition(), force));
}
void Rigidbody::Update(float deltaTime)
{
	if (isKinematic == false)
	{
		velocity = (velocity + (accumulatedForce * inverseMass) * deltaTime) * pow(damping, deltaTime);
		object->transform.Translate(velocity * deltaTime);

		angularVelocity = (angularVelocity + (GetWorldInertiaTensorInverse() * accumulatedTorque) * deltaTime) * pow(angularDamping, deltaTime);
		object->transform.Rotate(angularVelocity * deltaTime);
	}
	ClearAccumulatedForce();
}
void Rigidbody::SetKinematic(bool isKinematic)
{
	this->isKinematic = isKinematic;
}
void Rigidbody::SetEnabled(bool isEnabled)
{
	this->isEnabled = isEnabled;
}
void Rigidbody::SetInverseMass(float inverseMass)
{
	this->inverseMass = inverseMass;
}
void Rigidbody::SetDamping(float damping)
{
	this->damping = damping;
}
void Rigidbody::SetAngularDamping(float angularDamping)
{
	this->angularDamping = angularDamping;
}
void Rigidbody::SetVelocity(Vector3 velocity)
{
	this->velocity = velocity;
}
void Rigidbody::AddVelocity(Vector3 velocity)
{
	this->velocity += velocity;
}
void Rigidbody::SetAngularVelocity(Vector3 angularVelocity)
{
	this->angularVelocity = angularVelocity;
}
void Rigidbody::AddAngularVelocity(Vector3 angularVelocity)
{
	this->angularVelocity += angularVelocity;
}
void Rigidbody::SetAccumulatedForce(Vector3 accumulatedForce)
{
	this->accumulatedForce = accumulatedForce;
}
void Rigidbody::SetAccumulatedTorque(Vector3 accumulatedTorque)
{
	this->accumulatedTorque = accumulatedTorque;
}
void Rigidbody::SetInertiaTensor(Matrix4x4 inertiaTensor)
{
	this->inertiaTensor = inertiaTensor;
}
void Rigidbody::ClearAccumulatedForce()
{
	accumulatedForce = Vector3::Zero();
	accumulatedTorque = Vector3::Zero();
}
bool Rigidbody::IsKinematic() { return isKinematic; }
bool Rigidbody::IsEnabled() { return isEnabled; }
float Rigidbody::GetInverseMass(){return inverseMass;}
float Rigidbody::GetDamping(){return damping;}
float Rigidbody::GetangularDamping(){return angularDamping;}
Vector3 Rigidbody::GetVelocity(){return velocity;}
Vector3 Rigidbody::GetAngularVelocity(){return angularVelocity;}
Vector3 Rigidbody::GetAccumulatedForce(){return accumulatedForce;}
Vector3 Rigidbody::GetAccumulatedTorque(){return accumulatedTorque;}
Matrix4x4 Rigidbody::GetInertiaTensor(){return inertiaTensor;}

Matrix4x4 Rigidbody::GetWorldInertiaTensorInverse()
{
	//Matrix4x4 iitBody = inertiaTensor;
	//Matrix4x4 iitWorld = Matrix4x4::Identity();
	//Matrix4x4 rotmat = object->transform.GetRotationMatrix();
	//float t4 = rotmat.m00 * iitBody.m00 +
	//	rotmat.m01 * iitBody.m10 +
	//	rotmat.m02 * iitBody.m20;
	//float t9 = rotmat.m00 * iitBody.m01 +
	//	rotmat.m01 * iitBody.m11 +
	//	rotmat.m02 * iitBody.m21;
	//float t14 = rotmat.m00 * iitBody.m02 +
	//	rotmat.m01 * iitBody.m12 +
	//	rotmat.m02 * iitBody.m22;
	//float t28 = rotmat.m10 * iitBody.m00 +
	//	rotmat.m11 * iitBody.m10 +
	//	rotmat.m12 * iitBody.m20;
	//float t33 = rotmat.m10 * iitBody.m01 +
	//	rotmat.m11 * iitBody.m11 +
	//	rotmat.m12 * iitBody.m21;
	//float t38 = rotmat.m10 * iitBody.m02 +
	//	rotmat.m11 * iitBody.m12 +
	//	rotmat.m12 * iitBody.m22;
	//float t52 = rotmat.m20 * iitBody.m00 +
	//	rotmat.m21 * iitBody.m10 +
	//	rotmat.m22 * iitBody.m20;
	//float t57 = rotmat.m20 * iitBody.m01 +
	//	rotmat.m21 * iitBody.m11 +
	//	rotmat.m22 * iitBody.m21;
	//float t62 = rotmat.m20 * iitBody.m02 +
	//	rotmat.m21 * iitBody.m12 +
	//	rotmat.m22 * iitBody.m22;
	//iitWorld.m00 = t4 * rotmat.m00 +
	//	t9 * rotmat.m01 +
	//	t14 * rotmat.m02;
	//iitWorld.m01 = t4 * rotmat.m10 +
	//	t9 * rotmat.m11 +
	//	t14 * rotmat.m12;
	//iitWorld.m02 = t4 * rotmat.m20 +
	//	t9 * rotmat.m21 +
	//	t14 * rotmat.m22;
	//iitWorld.m10 = t28 * rotmat.m00 +
	//	t33 * rotmat.m01 +
	//	t38 * rotmat.m02;
	//iitWorld.m11 = t28 * rotmat.m10 +
	//	t33 * rotmat.m11 +
	//	t38 * rotmat.m12;
	//iitWorld.m12 = t28 * rotmat.m20 +
	//	t33 * rotmat.m21 +
	//	t38 * rotmat.m22;
	//iitWorld.m20 = t52 * rotmat.m00 +
	//	t57 * rotmat.m01 +
	//	t62 * rotmat.m02;
	//iitWorld.m21 = t52 * rotmat.m10 +
	//	t57 * rotmat.m11 +
	//	t62 * rotmat.m12;
	//iitWorld.m22 = t52 * rotmat.m20 +
	//	t57 * rotmat.m21 +
	//	t62 * rotmat.m22;

	//return iitWorld;
	return Matrix4x4(object->transform.GetRotationMatrix() * inertiaTensor.Inverse() * object->transform.GetRotationMatrix().Transpose());
}

// 힘 = 질량 * 가속도
// 임펄스 힘 = 질량 * 속도
// 토크 = 관성텐서 * 각가속도
// 임펄스 토크 = 관성텐서 * 각속도
// 각속도 = 관성텐서^-1 * (상대위치 X 임펄스 힘) 인듯..?