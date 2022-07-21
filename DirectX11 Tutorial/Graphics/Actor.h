#pragma once
#include "Object.h"

class Actor : public Object
{
public:
	Actor() {}
	bool Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<VSConstantBuffer>& vsConstantBuffer, aiColor3D defaultColor);
	void Draw(const XMMATRIX& viewProjectionMatrix) override;

	Model model;
	Rigidbody rigidbody;
};