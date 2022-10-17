#pragma once
#include "Object.h"

class Light : public Object
{
public:
	Light() {}
	void Draw(Matrix4x4 viewProjectionMatrix);
	void SetContantBuffer(ConstantBuffer<PSConstantBuffer>& psConstantBuffer, Vector3& cameraPosition);
	void SetStrength(float strength);
	
	float GetStrength();

private:
	float m_strength = 1.0f;
};