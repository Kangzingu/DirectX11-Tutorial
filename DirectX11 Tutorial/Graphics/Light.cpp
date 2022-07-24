#include "Light.h"

bool Light::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<VSConstantBuffer>& vsConstantBuffer, aiColor3D defaultColor)
{
	model.Initialize("Assets/Objects/light.fbx", device, deviceContext, vsConstantBuffer, defaultColor);
	return true;
}
void Light::Draw(Matrix4x4 viewProjectionMatrix)
{
	model.Draw(this->transform.GetWorldMatrix(), viewProjectionMatrix);
}
