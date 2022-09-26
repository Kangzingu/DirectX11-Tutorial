#pragma once
#include <DirectXMath.h>

struct Vertex
{
public:
	Vertex()
	{
		position = Vector3::Zero();
		texCoord = Vector2::Zero();
		normal = Vector3::Up();
	}
	Vertex(float x, float y, float z, float u, float v, float nx, float ny, float nz) : position(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}
	Vector3 position;
	Vector2 texCoord;
	Vector3 normal;
};