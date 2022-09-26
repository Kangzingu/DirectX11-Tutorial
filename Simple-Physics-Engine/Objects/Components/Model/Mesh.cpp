#include "Mesh.h"

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex>& vertices, std::vector<UINT>& indices, std::vector<Texture> textures, Matrix4x4 transformMatrix)
{
	m_deviceContext = deviceContext;
	m_vertexbuffer.Initialize(device, vertices.data(), vertices.size());
	m_indexbuffer.Initialize(device, indices.data(), indices.size());
	m_textures = textures;
	m_worldMatrix = transformMatrix;
	m_vertices = vertices;
	m_indices = indices;

	m_worldSpaceVerticesPosition.clear();
	for (int i = 0; i < m_vertices.size(); i++)
	{
		m_worldSpaceVerticesPosition.push_back(m_vertices[i].position);
	}
}
Mesh::Mesh(const Mesh& mesh)
{
	m_deviceContext = mesh.m_deviceContext;
	m_vertexbuffer = mesh.m_vertexbuffer;
	m_indexbuffer = mesh.m_indexbuffer;
	m_textures = mesh.m_textures;
	m_worldMatrix = mesh.m_worldMatrix;
	m_vertices = mesh.m_vertices;
	m_indices = mesh.m_indices;

	m_worldSpaceVerticesPosition.clear();
	for (int i = 0; i < m_vertices.size(); i++)
	{
		m_worldSpaceVerticesPosition.push_back(m_vertices[i].position);
	}
}
const void Mesh::Draw()
{
	UINT offset = 0;
	for (int i = 0; i < m_textures.size(); i++)
	{
		if (m_textures[i].GetType() == aiTextureType::aiTextureType_DIFFUSE)
		{
			m_deviceContext->PSSetShaderResources(0, 1, m_textures[i].GetTextureResourceViewAddress());
			break;
		}
	}
	m_deviceContext->IASetVertexBuffers(0, 1, m_vertexbuffer.GetAddressOf(), m_vertexbuffer.GetAddressOfStride(), &offset);
	m_deviceContext->IASetIndexBuffer(m_indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_deviceContext->DrawIndexed(m_indexbuffer.GetNumofIndex(), 0, 0);
}

void Mesh::UpdateWorldSpaceVertices(Matrix4x4& worldMatrix)
{
	for (int i = 0; i < m_vertices.size(); i++)
	{
		m_worldSpaceVerticesPosition[i] = worldMatrix * m_vertices[i].position;
	}
}
