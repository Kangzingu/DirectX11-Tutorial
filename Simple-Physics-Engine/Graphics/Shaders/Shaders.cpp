#include "Shaders.h"

void VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
{
	ERROR_IF_FAILED(D3DReadFileToBlob(shaderPath.c_str(), m_shaderBuffer.GetAddressOf()), "���ؽ� ���̴��� �ε��ϴµ� �����߽��ϴ�");
	ERROR_IF_FAILED(device->CreateVertexShader(m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), NULL, m_shader.GetAddressOf()), "���ؽ� ���̴��� ����µ� �����߽��ϴ�");
	ERROR_IF_FAILED(device->CreateInputLayout(layoutDesc, numElements, m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), m_inputLayout.GetAddressOf()), "Input Layout ������ �����߽��ϴ�");
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
	ERROR_IF_FAILED(D3DReadFileToBlob(shaderPath.c_str(), m_shaderBuffer.GetAddressOf()), "�ȼ� ���̴��� �ε��ϴµ� �����߽��ϴ�");
	ERROR_IF_FAILED(device->CreatePixelShader(m_shaderBuffer.Get()->GetBufferPointer(), m_shaderBuffer.Get()->GetBufferSize(), NULL, m_shader.GetAddressOf()), "�ȼ� ���̴��� ����µ� �����߽��ϴ�");
}
ID3D11PixelShader* PixelShader::GetShader()
{
	return m_shader.Get();
}
ID3D10Blob* PixelShader::GetBuffer()
{
	return m_shaderBuffer.Get();
}