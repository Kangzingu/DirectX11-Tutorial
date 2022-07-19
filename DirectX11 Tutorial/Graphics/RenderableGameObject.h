#pragma once
#include "Object.h"

class RenderableGameObject : public Object
{
public:
	bool Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<VSConstantBuffer>& vsConstantBuffer, aiColor3D defaultColor);

	void Draw(const XMMATRIX& viewProjectionMatrix);
	Rigidbody rigidbody;

protected:
	Model model;
};