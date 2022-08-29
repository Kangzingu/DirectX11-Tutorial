#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include "Texture.h"
#include "../Utils/ErrorLogger.h"

Texture::Texture(ID3D11Device* device, const Color& color, aiTextureType type)
{
	Initialize1x1ColorTexture(device, color, type);
}

Texture::Texture(ID3D11Device* device, const Color* colorData, UINT width, UINT height, aiTextureType type)
{
	InitializeColorTexture(device, colorData, width, height, type);
}

Texture::Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type)
{
	m_type = type;
	if (StringHelper::GetFileExtension(filePath) == ".dds")
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(device, StringHelper::StringToWString(filePath).c_str(), m_texture.GetAddressOf(), m_textureView.GetAddressOf());
		if (FAILED(hr))
		{
			Initialize1x1ColorTexture(device, Color::GetUnloadedTextureColor(), type);
		}
		return;
	}
	else
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringHelper::StringToWString(filePath).c_str(), m_texture.GetAddressOf(), m_textureView.GetAddressOf());
		if (FAILED(hr))
		{
			Initialize1x1ColorTexture(device, Color::GetUnloadedTextureColor(), type);
		}
		return;
	}
}

Texture::Texture(ID3D11Device* device, const uint8_t* pData, size_t size, aiTextureType type)
{
	m_type = type;
	HRESULT hr = DirectX::CreateWICTextureFromMemory(device, pData, size, m_texture.GetAddressOf(), m_textureView.GetAddressOf());
	ERROR_IF_FAILED(hr, "메모리로 부터 텍스쳐를 생성하는데 실패했습니다");
}

aiTextureType Texture::GetType()
{
	return m_type;
}

ID3D11ShaderResourceView* Texture::GetTextureResourceView()
{
	return m_textureView.Get();
}

ID3D11ShaderResourceView** Texture::GetTextureResourceViewAddress()
{
	return m_textureView.GetAddressOf();
}

void Texture::Initialize1x1ColorTexture(ID3D11Device* device, const Color& colorData, aiTextureType type)
{
	InitializeColorTexture(device, &colorData, 1, 1, type);
}
void Texture::InitializeColorTexture(ID3D11Device* device, const Color* colorData, UINT width, UINT height, aiTextureType type)
{
	m_type = type;
	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	ID3D11Texture2D* p2DTexture = nullptr;
	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = colorData;
	initialData.SysMemPitch = width * sizeof(Color);
	HRESULT hr = device->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
	ERROR_IF_FAILED(hr, "컬러 데이터로 부터 텍스쳐를 생성하는데 실패했습니다");
	m_texture = static_cast<ID3D11Texture2D*>(p2DTexture);
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
	hr = device->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_textureView.GetAddressOf());
	ERROR_IF_FAILED(hr, "텍스쳐에 대한 쉐이더 리소스 뷰 생성에 실패했습니다");

}