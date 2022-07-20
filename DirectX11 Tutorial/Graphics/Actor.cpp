#include "Actor.h"

bool Actor::Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<VSConstantBuffer>& vsConstantBuffer, aiColor3D defaultColor)
{
	model.Initialize(filePath, device, deviceContext, vsConstantBuffer, defaultColor);
	rigidbody.Initialize(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), 0.95f, 1.0f, false);
	this->transform.SetPosition(0.0f, 0.0f, 0.0f);
	this->transform.SetRotation(0.0f, 0.0f, 0.0f);
	this->transform.SetScale(1.0f, 1.0f, 1.0f);
	this->transform.UpdateMatrix();
	return true;
}

void Actor::Draw(const XMMATRIX& viewProjectionMatrix)
{
	model.Draw(this->transform.worldMatrix, viewProjectionMatrix);
}
