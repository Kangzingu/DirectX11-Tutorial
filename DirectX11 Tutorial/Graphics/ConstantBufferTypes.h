#pragma once
#include <DirectXMath.h>

// Constant Buffer for Vertex Shader
struct CB_VS_vertexshader
{
	// �̰� 16byte ������ �� �е� �־�����ϰ� �׷��� �־��µ�.. �۵� �̻��ϰ� �ȴٸ� ��Ʃ�� "C++ DirectX 11 Engine Tutorial 26 - Constant Buffer Packing Alignment" ����
	DirectX::XMMATRIX mat;
};