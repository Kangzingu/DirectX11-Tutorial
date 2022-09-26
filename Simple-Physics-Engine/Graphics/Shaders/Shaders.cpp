#include "Shaders.h"

void VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
{
	ERROR_IF_FAILED(D3DReadFileToBlob(shaderPath.c_str(), m_shaderBuffer.GetAddressOf()), "버텍스 쉐이더를 로드하는데 실패했습니다");
	ERROR_IF_FAILED(device->CreateVertexShader(m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), NULL, m_shader.GetAddressOf()), "버텍스 쉐이더를 만드는데 실패했습니다");
	ERROR_IF_FAILED(device->CreateInputLayout(layoutDesc, numElements, m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), m_inputLayout.GetAddressOf()), "Input Layout 생성에 실패했습니다");
}
ID3D11VertexShader* VertexShader::GetShader()
{
	return m_shader.Get();
}
ID3D10Blob* VertexShader::GetBuffer()
{
	return m_shaderBuffer.Get();
}
ID3D11InputLayout* VertexShader::GetInputLayout()
{
	return m_inputLayout.Get();
}
void PixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderPath)
{
	ERROR_IF_FAILED(D3DReadFileToBlob(shaderPath.c_str(), m_shaderBuffer.GetAddressOf()), "픽셀 쉐이더를 로드하는데 실패했습니다");
	ERROR_IF_FAILED(device->CreatePixelShader(m_shaderBuffer.Get()->GetBufferPointer(), m_shaderBuffer.Get()->GetBufferSize(), NULL, m_shader.GetAddressOf()), "픽셀 쉐이더를 만드는데 실패했습니다");
}
ID3D11PixelShader* PixelShader::GetShader()
{
	return m_shader.Get();
}
ID3D10Blob* PixelShader::GetBuffer()
{
	return m_shaderBuffer.Get();
}