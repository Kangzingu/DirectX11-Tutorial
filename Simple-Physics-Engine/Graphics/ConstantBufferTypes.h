#pragma once
#include <DirectXMath.h>

// Constant Buffer for Vertex Shader
struct VSConstantBuffer
{
	// 이거 16byte 단위로 뭐 패딩 넣어줘야하고 그렇거 있었는데.. 작동 이상하게 된다면 유튜브 "C++ DirectX 11 Engine Tutorial 26 - Constant Buffer Packing Alignment" 참고
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};
struct VSConstantBuffer2D
{
	// 이거 16byte 단위로 뭐 패딩 넣어줘야하고 그렇거 있었는데.. 작동 이상하게 된다면 유튜브 "C++ DirectX 11 Engine Tutorial 26 - Constant Buffer Packing Alignment" 참고
	DirectX::XMMATRIX wvpMatrix;
};

struct PSConstantBuffer
{
	// 16byte
	DirectX::XMFLOAT3 ambientLightColor;
	float ambientLightStrength;
	// 16byte
	DirectX::XMFLOAT3 dynamicLightColor;
	float dynamicLightStrength;
	// 16byte
	DirectX::XMFLOAT3 dynamicLightPosition;
	float dynamicLightAttenA;
	float dynamicLightAttenB;
	float dynamicLightAttenC;
};