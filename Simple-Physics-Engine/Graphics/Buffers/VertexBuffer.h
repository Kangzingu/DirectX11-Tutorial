#ifndef VertexBuffer_h__
#define VertexBuffer_h__
#include <d3d11.h>
#include <wrl/client.h>
using namespace Microsoft::WRL;

template<class T>
class VertexBuffer
{
private:
	ComPtr<ID3D11Buffer> m_buffer = nullptr;

	UINT m_numofVertex = 0;
	UINT m_stride = sizeof(T);

public:
	VertexBuffer() {};
	void Initialize(ID3D11Device* device, T* data, UINT numofVertex)
	{
		if (m_buffer.Get() != nullptr)
			m_buffer.Reset();

		m_numofVertex = numofVertex;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		D3D11_BUFFER_DESC vertexBufferDescription;
		ZeroMemory(&vertexBufferDescription, sizeof(vertexBufferDescription));
		vertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDescription.ByteWidth = m_stride * numofVertex;
		vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDescription.CPUAccessFlags = 0;
		vertexBufferDescription.MiscFlags = 0;

		ERROR_IF_FAILED(device->CreateBuffer(&vertexBufferDescription, &vertexBufferData, m_buffer.GetAddressOf()), "버텍스 버퍼 생성에 실패했습니다");
	}
	ID3D11Buffer* Get() const
	{
		return m_buffer.Get();
	}
	ID3D11Buffer* const* GetAddressOf() const
	{
		return m_buffer.GetAddressOf();
	}
	UINT GetNumofVertex() const
	{
		return m_numofVertex;
	}
	const UINT GetStride() const
	{
		return m_stride;
	}
	const UINT* GetAddressOfStride() const
	{
		return &m_stride;
	}
};
#endif