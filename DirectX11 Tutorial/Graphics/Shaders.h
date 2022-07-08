#pragma once
#include "../ErrorLogger.h"
#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

class VertexShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements);
	ID3D11VertexShader* GetShader();
	ID3D10Blob* GetBuffer();
	ID3D11InputLayout* GetInputLayout();
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;// DX 11에서 정한 형식에 맞는 쉐이더, Blob을 가공해 만든 느낌
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer;// 임시로? raw한? 쉐이더 정보를 담는 느낌
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

class PixelShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath);
	ID3D11PixelShader* GetShader();
	ID3D10Blob* GetBuffer();
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;// DX 11에서 정한 형식에 맞는 쉐이더, Blob을 가공해 만든 느낌
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer;// 임시로? raw한? 쉐이더 정보를 담는 느낌
};