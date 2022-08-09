#include "Light.h"

void Light::Draw(Matrix4x4 viewProjectionMatrix)
{
	m_model.Draw(m_transform.GetWorldMatrix(), viewProjectionMatrix);
}
void Light::SetContantBuffer(ConstantBuffer<PSConstantBuffer>& psConstantBuffer, Vector3& cameraPosition)
{
	psConstantBuffer.data.lightPosition = m_transform.GetPosition();
	psConstantBuffer.data.lightStrength = m_strength;
	psConstantBuffer.data.cameraPosition = cameraPosition;
}
void Light::SetStrength(float strength)
{
	m_strength = strength;
}
