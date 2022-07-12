#include "Mesh.h"

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex>& vertices, std::vector<DWORD>& indices)
{
	this->deviceContext = deviceContext;

	HRESULT hr = this->vertexbuffer.Initialize(device, vertices.data(), vertices.size());
	COM_ERROR_IF_FAILED(hr, "메시를 위한 버텍스 버퍼 생성에 실패했습니다");

	hr = this->indexbuffer.Initialize(device, indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, "메시를 위한 인덱스 버퍼 생성에 실패했습니다");
}

Mesh::Mesh(const Mesh& mesh)
{
	this->deviceContext = mesh.deviceContext;
	this->indexbuffer = mesh.indexbuffer;
	this->vertexbuffer = mesh.vertexbuffer;
}

void Mesh::Draw()
{
	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexbuffer.GetAddressOf(), this->vertexbuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(this->indexbuffer.BufferSize(), 0, 0);
}
