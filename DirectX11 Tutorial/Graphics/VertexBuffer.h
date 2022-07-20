/* 끝 */
#ifndef VertexBuffer_h__
#define VertexBuffer_h__
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

template<class T>
class VertexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT stride = sizeof(T);
	UINT numofVertex = 0;

public:
	void Initialize(ID3D11Device* device, T* data, UINT vertexCount)
	{
		if (buffer.Get() != nullptr)
			buffer.Reset();
		this->numofVertex = vertexCount;
		D3D11_BUFFER_DESC vertexBufferDescription;
		ZeroMemory(&vertexBufferDescription, sizeof(vertexBufferDescription));
		vertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDescription.ByteWidth = stride * vertexCount;
		vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDescription.CPUAccessFlags = 0;
		vertexBufferDescription.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;
		ERROR_IF_FAILED(device->CreateBuffer(&vertexBufferDescription, &vertexBufferData, this->buffer.GetAddressOf()), "버텍스 버퍼 생성에 실패했습니다");
	}
	VertexBuffer() {}
	VertexBuffer(const VertexBuffer<T>& input)
	{
		this->buffer = input.buffer;
		this->numofVertex = input.numofVertex;
		this->stride = input.stride;
	}
	VertexBuffer<T>& operator=(const VertexBuffer<T>& input)
	{
		this->buffer = input.buffer;
		this->numofVertex = input.numofVertex;
		this->stride = input.stride;
		return *this;
	}
	ID3D11Buffer* Get() const
	{
		return buffer.Get();
	}
	ID3D11Buffer* const* GetAddressOf() const
	{
		return buffer.GetAddressOf();
	}
	UINT VertexCount() const
	{
		return this->numofVertex;
	}
	const UINT Stride() const
	{
		return this->stride;
	}
	const UINT* StridePtr() const
	{
		return &this->stride;
	}
};
#endif