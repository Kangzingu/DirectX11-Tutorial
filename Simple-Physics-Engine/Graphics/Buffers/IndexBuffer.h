#ifndef IndexBuffer_h__
#define IndexBuffer_h__
#include <d3d11.h>
#include <wrl/client.h>
using namespace Microsoft::WRL;

class IndexBuffer
{
private:
	ComPtr<ID3D11Buffer> m_buffer = nullptr;

	UINT m_numofIndex = 0;

public:
	IndexBuffer() {};
	void Initialize(ID3D11Device* device, UINT* data, UINT numofIndex)
	{
		if (m_buffer.Get() != nullptr)
			m_buffer.Reset();

		m_numofIndex = numofIndex;

		D3D11_SUBRESOURCE_DATA indexBufferData;
		ZeroMemory(&indexBufferData, sizeof(indexBufferData));
		indexBufferData.pSysMem = data;

		D3D11_BUFFER_DESC indexBufferDescription;
		ZeroMemory(&indexBufferDescription, sizeof(indexBufferDescription));
		indexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDescription.ByteWidth = sizeof(UINT) * numofIndex;
		indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDescription.CPUAccessFlags = 0;
		indexBufferDescription.MiscFlags = 0;
		
		ERROR_IF_FAILED(device->CreateBuffer(&indexBufferDescription, &indexBufferData, m_buffer.GetAddressOf()), "인덱스 버퍼 생성에 실패했습니다");
	}
	ID3D11Buffer* Get() const
	{
		return m_buffer.Get();
	}
	ID3D11Buffer* const* GetAddressOf() const
	{
		return m_buffer.GetAddressOf();
	}
	UINT GetNumofIndex() const
	{
		return m_numofIndex;
	}
};
#endif