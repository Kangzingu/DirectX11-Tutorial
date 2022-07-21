#include "Mesh.h"

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex3D>& vertices, std::vector<DWORD>& indices, std::vector<Texture> textures, Matrix4x4 transformMatrix)
{
	this->deviceContext = deviceContext;
	this->vertexbuffer.Initialize(device, vertices.data(), vertices.size());
	this->indexbuffer.Initialize(device, indices.data(), indices.size());
	this->textures = textures;
	this->worldMatrix = transformMatrix;
}
Mesh::Mesh(const Mesh& mesh)
{
	this->deviceContext = mesh.deviceContext;
	this->vertexbuffer = mesh.vertexbuffer;
	this->indexbuffer = mesh.indexbuffer;
	this->textures = mesh.textures;
	this->worldMatrix = mesh.worldMatrix;
}
const void Mesh::Draw()
{
	UINT offset = 0;
	for (int i = 0; i < this->textures.size(); i++)
	{
		if (this->textures[i].GetType() == aiTextureType::aiTextureType_DIFFUSE)
		{
			this->deviceContext->PSSetShaderResources(0, 1, textures[i].GetTextureResourceViewAddress());
			break;
		}
	}
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexbuffer.GetAddressOf(), this->vertexbuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(this->indexbuffer.IndexCount(), 0, 0);
}