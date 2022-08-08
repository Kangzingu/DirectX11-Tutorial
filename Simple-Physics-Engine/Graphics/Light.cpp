#include "Light.h"

void Light::Draw(Matrix4x4 viewProjectionMatrix)
{
	model.Draw(this->transform.GetWorldMatrix(), viewProjectionMatrix);
}

void Light::SetAmbientLight(ConstantBuffer<PSConstantBuffer>& psConstantBuffer, Vector3 ambientColor, float ambientStrength)
{
	this->ambientColor = ambientColor;
	this->ambientStrength = ambientStrength;

	psConstantBuffer.data.ambientLightColor = this->ambientColor.ToXMFLOAT3();
	psConstantBuffer.data.ambientLightStrength = this->ambientStrength;
}

void Light::SetDynamicLight(ConstantBuffer<PSConstantBuffer>& psConstantBuffer, Vector3 dynamicColor, float dynamicStrength, float dynamicAttenA, float dynamicAttenB, float dynamicAttenC)
{
	this->dynamicColor = dynamicColor;
	this->dynamicStrength = dynamicStrength;
	this->dynamicAttenA = dynamicAttenA;
	this->dynamicAttenB = dynamicAttenB;
	this->dynamicAttenC = dynamicAttenC;

	psConstantBuffer.data.dynamicLightColor = this->dynamicColor.ToXMFLOAT3();
	psConstantBuffer.data.dynamicLightPosition = this->transform.GetPosition().ToXMFLOAT3();
	psConstantBuffer.data.dynamicLightStrength = this->dynamicStrength;
	psConstantBuffer.data.dynamicLightAttenA = this->dynamicAttenA;
	psConstantBuffer.data.dynamicLightAttenB = this->dynamicAttenB;
	psConstantBuffer.data.dynamicLightAttenC = this->dynamicAttenC;
}

void Light::SetContantBuffer(ConstantBuffer<PSConstantBuffer>& psConstantBuffer)
{
	psConstantBuffer.data.ambientLightColor = this->ambientColor.ToXMFLOAT3();
	psConstantBuffer.data.ambientLightStrength = this->ambientStrength;
	psConstantBuffer.data.dynamicLightColor = this->dynamicColor.ToXMFLOAT3();
	psConstantBuffer.data.dynamicLightPosition = this->transform.GetPosition().ToXMFLOAT3();
	psConstantBuffer.data.dynamicLightStrength = this->dynamicStrength;
	psConstantBuffer.data.dynamicLightAttenA = this->dynamicAttenA;
	psConstantBuffer.data.dynamicLightAttenB = this->dynamicAttenB;
	psConstantBuffer.data.dynamicLightAttenC = this->dynamicAttenC;
}