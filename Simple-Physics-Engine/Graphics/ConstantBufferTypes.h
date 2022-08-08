#pragma once
#include <DirectXMath.h>

// Constant Buffer for Vertex Shader
struct VSConstantBuffer
{
	// �̰� 16byte ������ �� �е� �־�����ϰ� �׷��� �־��µ�.. �۵� �̻��ϰ� �ȴٸ� ��Ʃ�� "C++ DirectX 11 Engine Tutorial 26 - Constant Buffer Packing Alignment" ����
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};
struct VSConstantBuffer2D
{
	// �̰� 16byte ������ �� �е� �־�����ϰ� �׷��� �־��µ�.. �۵� �̻��ϰ� �ȴٸ� ��Ʃ�� "C++ DirectX 11 Engine Tutorial 26 - Constant Buffer Packing Alignment" ����
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