#include "Actor.h"

void Actor::Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<VSConstantBuffer>& vsConstantBuffer, aiColor3D defaultColor)
{
	model.Initialize(filePath, device, deviceContext, vsConstantBuffer, defaultColor);
	rigidbody.Initialize(Vector3::Zero(), Vector3::Zero(), 0.95f, 3.0f, false);
}

void Actor::Draw(Matrix4x4 viewProjectionMatrix)
{
	model.Draw(this->transform.GetWorldMatrix(), viewProjectionMatrix);
}
