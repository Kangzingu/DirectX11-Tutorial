#pragma once
#include <DirectXMath.h>

// Constant Buffer for Vertex Shader
struct CB_VS_vertexshader
{
	// 이거 16byte 단위로 뭐 패딩 넣어줘야하고 그렇거 있었는데.. 작동 이상하게 된다면 유튜브 "C++ DirectX 11 Engine Tutorial 26 - Constant Buffer Packing Alignment" 참고
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};
struct CB_VS_vertexshader_2d
{
	// 이거 16byte 단위로 뭐 패딩 넣어줘야하고 그렇거 있었는데.. 작동 이상하게 된다면 유튜브 "C++ DirectX 11 Engine Tutorial 26 - Constant Buffer Packing Alignment" 참고
	DirectX::XMMATRIX wvpMatrix;
};

struct CB_PS_light
{
	// 16byte
	DirectX::XMFLOAT3 ambientLightColor;
	float ambientLightStrength;

	// 16byte
	DirectX::XMFLOAT3 dynamicLightColor;
	float dynamicLightStrength;

	// 16byte
	DirectX::XMFLOAT3 dynamicLightPosition;
	float dynamicLightAttenuation_a;

	float dynamicLightAttenuation_b;
	float dynamicLightAttenuation_c;
};