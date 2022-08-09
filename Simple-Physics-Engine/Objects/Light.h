#pragma once
#include "Object.h"

class Light : public Object
{
public:
	Light() {}
	void Draw(Matrix4x4 viewProjectionMatrix);
	void SetContantBuffer(ConstantBuffer<PSConstantBuffer>& psConstantBuffer);
	void SetAmbientLight(ConstantBuffer<PSConstantBuffer>& psConstantBuffer, Vector3 ambientColor, float ambientStrength);
	void SetDynamicLight(ConstantBuffer<PSConstantBuffer>& psConstantBuffer, Vector3 dynamicColor, float dynamicStrength, float dynamicAttenA, float dynamicAttenB, float dynamicAttenC);

	Vector3 m_ambientColor = Vector3::One();
	float m_ambientStrength = 0.5f;
	
	Vector3 m_dynamicColor = Vector3::One();
	float m_dynamicStrength = 3.0f;
	float m_dynamicAttenA = 1.0f;
	float m_dynamicAttenB = 0.1f;
	float m_dynamicAttenC = 0.1f;
};