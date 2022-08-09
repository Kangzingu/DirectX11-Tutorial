#include "Light.h"

void Light::Draw(Matrix4x4 viewProjectionMatrix)
{
	m_model.Draw(m_transform.GetWorldMatrix(), viewProjectionMatrix);
}

void Light::SetAmbientLight(ConstantBuffer<PSConstantBuffer>& psConstantBuffer, Vector3 ambientColor, float ambientStrength)
{
	m_ambientColor = ambientColor;
	m_ambientStrength = ambientStrength;

	psConstantBuffer.data.ambientLightColor = m_ambientColor.ToXMFLOAT3();
	psConstantBuffer.data.ambientLightStrength = m_ambientStrength;
}

void Light::SetDynamicLight(ConstantBuffer<PSConstantBuffer>& psConstantBuffer, Vector3 dynamicColor, float dynamicStrength, float dynamicAttenA, float dynamicAttenB, float dynamicAttenC)
{
	m_dynamicColor = dynamicColor;
	m_dynamicStrength = dynamicStrength;
	m_dynamicAttenA = dynamicAttenA;
	m_dynamicAttenB = dynamicAttenB;
	m_dynamicAttenC = dynamicAttenC;

	psConstantBuffer.data.dynamicLightColor = m_dynamicColor.ToXMFLOAT3();
	psConstantBuffer.data.dynamicLightPosition = m_transform.GetPosition().ToXMFLOAT3();
	psConstantBuffer.data.dynamicLightStrength = m_dynamicStrength;
	psConstantBuffer.data.dynamicLightAttenA = m_dynamicAttenA;
	psConstantBuffer.data.dynamicLightAttenB = m_dynamicAttenB;
	psConstantBuffer.data.dynamicLightAttenC = m_dynamicAttenC;
}

void Light::SetContantBuffer(ConstantBuffer<PSConstantBuffer>& psConstantBuffer)
{
	psConstantBuffer.data.ambientLightColor = m_ambientColor.ToXMFLOAT3();
	psConstantBuffer.data.ambientLightStrength = m_ambientStrength;
	psConstantBuffer.data.dynamicLightColor = m_dynamicColor.ToXMFLOAT3();
	psConstantBuffer.data.dynamicLightPosition = m_transform.GetPosition().ToXMFLOAT3();
	psConstantBuffer.data.dynamicLightStrength = m_dynamicStrength;
	psConstantBuffer.data.dynamicLightAttenA = m_dynamicAttenA;
	psConstantBuffer.data.dynamicLightAttenB = m_dynamicAttenB;
	psConstantBuffer.data.dynamicLightAttenC = m_dynamicAttenC;
}