#pragma once

#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <vector>

class Mesh
{
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex3D>& vertices, std::vector<DWORD>& indices, std::vector<Texture> textures, const DirectX::XMMATRIX& transformMatrix);
	Mesh(const Mesh& mesh);
	const void Draw();
	const DirectX::XMMATRIX& GetWorldMatrix();

private:
	ID3D11DeviceContext* deviceContext;
	VertexBuffer<Vertex3D> vertexbuffer;
	IndexBuffer indexbuffer;
	std::vector<Texture> textures;
	DirectX::XMMATRIX worldMatrix;
};