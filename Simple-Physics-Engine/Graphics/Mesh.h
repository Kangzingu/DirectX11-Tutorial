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
#include "../Utils/SimpleMath.h"

class Mesh
{
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture> textures, Matrix4x4 transformMatrix);
	Mesh(const Mesh& mesh);
	const void Draw();
	Matrix4x4 m_worldMatrix;
	vector<Vertex> m_vertices;
	vector<DWORD> m_indices;

private:
	ID3D11DeviceContext* m_deviceContext;
	VertexBuffer<Vertex> m_vertexbuffer;
	IndexBuffer m_indexbuffer;
	std::vector<Texture> m_textures;
};