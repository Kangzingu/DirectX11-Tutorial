#pragma once
#include "Object.h"

class Light : public Object
{
public:
	void Draw(Matrix4x4 viewProjectionMatrix);
	void SetContantBuffer(ConstantBuffer<PSConstantBuffer>& psConstantBuffer);
	void SetAmbientLight(ConstantBuffer<PSConstantBuffer>& psConstantBuffer, Vector3 ambientColor, float ambientStrength);
	void SetDynamicLight(ConstantBuffer<PSConstantBuffer>& psConstantBuffer, Vector3 dynamicColor, float dynamicStrength, float dynamicAttenA, float dynamicAttenB, float dynamicAttenC);

	Vector3 ambientColor = Vector3::One();
	float ambientStrength = 0.5f;
	
	Vector3 dynamicColor = Vector3::One();
	float dynamicStrength = 3.0f;
	float dynamicAttenA = 1.0f;
	float dynamicAttenB = 0.1f;
	float dynamicAttenC = 0.1f;
};