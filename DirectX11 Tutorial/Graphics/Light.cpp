#include "Light.h"

bool Light::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<VSConstantBuffer>& cb_vs_vertexshader, aiColor3D defaultColor)
{
	model.Initialize("Assets/Objects/light.fbx", device, deviceContext, cb_vs_vertexshader, defaultColor);
	this->transform.SetPosition(3.0f, 10.0f, 0.0f);
	this->transform.SetRotation(0.0f, 0.0f, 0.0f);
	this->transform.SetScale(1.0f, 1.0f, 1.0f);
	this->transform.UpdateMatrix();
	return true;
}
