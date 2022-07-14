#pragma once
#include <DirectXMath.h>

// Constant Buffer for Vertex Shader
struct CB_VS_vertexshader
{
	// �̰� 16byte ������ �� �е� �־�����ϰ� �׷��� �־��µ�.. �۵� �̻��ϰ� �ȴٸ� ��Ʃ�� "C++ DirectX 11 Engine Tutorial 26 - Constant Buffer Packing Alignment" ����
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};
struct CB_VS_vertexshader_2d
{
	// �̰� 16byte ������ �� �е� �־�����ϰ� �׷��� �־��µ�.. �۵� �̻��ϰ� �ȴٸ� ��Ʃ�� "C++ DirectX 11 Engine Tutorial 26 - Constant Buffer Packing Alignment" ����
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