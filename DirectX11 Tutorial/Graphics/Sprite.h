#pragma once
#include "GameObject2D.h"
#include "Texture.h"
#include "ConstantBuffer.h"
#include <string>
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Vertex.h"

class Sprite : public GameObject2D
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float width, float height, std::string spritePath, ConstantBuffer<VSConstantBuffer2D>& cb_vs_vertexshader_2d);
	void Draw(XMMATRIX orthoMatrix);
	float GetWidth();
	float GetHeight();
	
private:
	void UpdateMatrix() override;

	ConstantBuffer<VSConstantBuffer2D>* cb_vs_vertexshader_2d = nullptr;
	XMMATRIX worldMatrix = XMMatrixIdentity();

	std::unique_ptr<Texture> texture;
	ID3D11DeviceContext* deviceContext = nullptr;

	IndexBuffer indices;
	VertexBuffer<Vertex2D> vertices;
};