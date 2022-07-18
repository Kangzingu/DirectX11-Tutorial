#include "Light.h"

bool Light::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, aiColor3D defaultColor)
{
	model.Initialize("Assets/Objects/light.fbx", device, deviceContext, cb_vs_vertexshader, defaultColor);
	this->SetPosition(0.0f, -10.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->SetScale(1.0f, 1.0f, 1.0f);
	this->UpdateMatrix();
	return true;
}
