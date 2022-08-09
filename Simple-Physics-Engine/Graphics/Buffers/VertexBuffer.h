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
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	UINT m_stride = sizeof(T);
	UINT m_numofVertex = 0;

public:
	void Initialize(ID3D11Device* device, T* data, UINT vertexCount)
	{
		if (m_buffer.Get() != nullptr)
			m_buffer.Reset();
		m_numofVertex = vertexCount;
		D3D11_BUFFER_DESC vertexBufferDescription;
		ZeroMemory(&vertexBufferDescription, sizeof(vertexBufferDescription));
		vertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDescription.ByteWidth = m_stride * vertexCount;
		vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDescription.CPUAccessFlags = 0;
		vertexBufferDescription.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;
		ERROR_IF_FAILED(device->CreateBuffer(&vertexBufferDescription, &vertexBufferData, m_buffer.GetAddressOf()), "버텍스 버퍼 생성에 실패했습니다");
	}
	VertexBuffer() {}
	VertexBuffer(const VertexBuffer<T>& input)
	{
		m_buffer = input.m_buffer;
		m_numofVertex = input.m_numofVertex;
		m_stride = input.m_stride;
	}
	VertexBuffer<T>& operator=(const VertexBuffer<T>& input)
	{
		m_buffer = input.m_buffer;
		m_numofVertex = input.m_numofVertex;
		m_stride = input.m_stride;
		return *this;
	}
	ID3D11Buffer* Get() const
	{
		return m_buffer.Get();
	}
	ID3D11Buffer* const* GetAddressOf() const
	{
		return m_buffer.GetAddressOf();
	}
	UINT VertexCount() const
	{
		return m_numofVertex;
	}
	const UINT Stride() const
	{
		return m_stride;
	}
	const UINT* StridePtr() const
	{
		return &m_stride;
	}
};
#endif