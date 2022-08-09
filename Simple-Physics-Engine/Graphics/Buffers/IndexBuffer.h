/* 끝 */
#ifndef IndicesBuffer_h__
#define IndicesBuffer_h__
#include <d3d11.h>
#include <wrl/client.h>

class IndexBuffer
{
private:
	IndexBuffer(const IndexBuffer& rhs);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	UINT m_numofIndex = 0;

public:
	void Initialize(ID3D11Device* device, DWORD* data, UINT indexCount)
	{
		if (m_buffer.Get() != nullptr)
			m_buffer.Reset();
		m_numofIndex = indexCount;
		D3D11_BUFFER_DESC indexBufferDescription;
		ZeroMemory(&indexBufferDescription, sizeof(indexBufferDescription));
		indexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDescription.ByteWidth = sizeof(DWORD) * indexCount;
		indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDescription.CPUAccessFlags = 0;
		indexBufferDescription.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA indexBufferData;
		ZeroMemory(&indexBufferData, sizeof(indexBufferData));
		indexBufferData.pSysMem = data;
		ERROR_IF_FAILED(device->CreateBuffer(&indexBufferDescription, &indexBufferData, m_buffer.GetAddressOf()), "인덱스 버퍼 생성에 실패했습니다");
	}
	IndexBuffer() {};
	ID3D11Buffer* Get() const
	{
		return m_buffer.Get();
	}
	ID3D11Buffer* const* GetAddressOf() const
	{
		return m_buffer.GetAddressOf();
	}
	UINT IndexCount() const
	{
		return m_numofIndex;
	}
};
#endif