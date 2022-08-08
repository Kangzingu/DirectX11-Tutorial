#include "Shaders.h"

void VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
{
	ERROR_IF_FAILED(D3DReadFileToBlob(shaderPath.c_str(), this->shaderBuffer.GetAddressOf()),
						"버텍스 쉐이더를 로드하는데 실패했습니다");
	ERROR_IF_FAILED(device->CreateVertexShader(this->shaderBuffer->GetBufferPointer(), this->shaderBuffer->GetBufferSize(), NULL, this->shader.GetAddressOf()),
						"버텍스 쉐이더를 만드는데 실패했습니다");
	ERROR_IF_FAILED(device->CreateInputLayout(layoutDesc, numElements, this->shaderBuffer->GetBufferPointer(), this->shaderBuffer->GetBufferSize(), this->inputLayout.GetAddressOf()),
						"Input Layout 생성에 실패했습니다");
}
ID3D11VertexShader* VertexShader::GetShader()
{
	return this->shader.Get();
}
ID3D10Blob* VertexShader::GetBuffer()
{
	return this->shaderBuffer.Get();
}
ID3D11InputLayout* VertexShader::GetInputLayout()
{
	return this->inputLayout.Get();
}
void PixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderPath)
{
	ERROR_IF_FAILED(D3DReadFileToBlob(shaderPath.c_str(), this->shaderBuffer.GetAddressOf()),
						"픽셀 쉐이더를 로드하는데 실패했습니다");
	ERROR_IF_FAILED(device->CreatePixelShader(this->shaderBuffer.Get()->GetBufferPointer(), this->shaderBuffer.Get()->GetBufferSize(), NULL, this->shader.GetAddressOf()),
						"픽셀 쉐이더를 만드는데 실패했습니다");
}
ID3D11PixelShader* PixelShader::GetShader()
{
	return this->shader.Get();
}
ID3D10Blob* PixelShader::GetBuffer()
{
	return this->shaderBuffer.Get();
}