#pragma once
#include <DirectXMath.h>

struct Vertex
{
public:
	Vertex() {}
	Vertex(float x, float y, float z, float u, float v, float nx, float ny, float nz) : position(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 normal;
};