#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include "../../Utils/ErrorLogger.h"

class VertexShader
{
public:
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements);
	ID3D11VertexShader* GetShader();
	ID3D10Blob* GetBuffer();
	ID3D11InputLayout* GetInputLayout();
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shader;// DX 11에서 정한 형식에 맞는 쉐이더, Blob을 가공해 만든 느낌
	Microsoft::WRL::ComPtr<ID3D10Blob> m_shaderBuffer;// 임시로? raw한? 쉐이더 정보를 담는 느낌
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};

class PixelShader
{
public:
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderPath);
	ID3D11PixelShader* GetShader();
	ID3D10Blob* GetBuffer();
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_shader;// DX 11에서 정한 형식에 맞는 쉐이더, Blob을 가공해 만든 느낌
	Microsoft::WRL::ComPtr<ID3D10Blob> m_shaderBuffer;// 임시로? raw한? 쉐이더 정보를 담는 느낌
};