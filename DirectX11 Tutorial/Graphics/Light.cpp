#include "Light.h"

bool Light::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<VSConstantBuffer>& vsConstantBuffer, aiColor3D defaultColor)
{
	model.Initialize("Assets/Objects/light.fbx", device, deviceContext, vsConstantBuffer, defaultColor);
	this->transform.position = Vector3( 0.0f, 0.0f, 0.0f );
	this->transform.rotation = Vector3(0.0f, 0.0f, 0.0f);
	this->transform.scale = Vector3(1.0f, 1.0f, 1.0f);
	this->transform.UpdateMatrix();
	return true;
}
void Light::Draw(Matrix4x4 viewProjectionMatrix)
{
	model.Draw(this->transform.worldMatrix, viewProjectionMatrix);
}
