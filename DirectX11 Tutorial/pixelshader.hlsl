cbuffer lightBuffer : register(b0)
{
	float3 ambientLightColor;
	float ambientLightStrength;
	float3 dynamicLightColor;
	float dynamicLightStrength;
	float3 dynamicLightPosition;

	float dynamicLightAttenA;
	float dynamicLightAttenB;
	float dynamicLightAttenC;
}

struct PS_INPUT
{
	// "SV_POSITION"������ "SV"�� system value ��� ��, �ٸ� ���̴��� �ѱ� ���� ���ϴ� ��? ��� position���� �ȼ� ���̴��� �ѱ�ϱ� "SV_POSITION"���� ����
	float4 inPosition:SV_POSITION;
	float2 inTexCoord:TEXCOORD;
	float3 inNormal:NORMAL;
	float3 inWorldPos:WORLD_POSITION;
};

// Texture: �̹���
// Sampler: �̹����� �ҷ����� ���(�ε��� ����� �� ��������, ���� ��� ��)
Texture2D objTexture : TEXTURE: register(t0);
SamplerState objSamplerState : SAMPLER: register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
	//float3 sampleColor = input.inNormal;
	float3 ambientLight = ambientLightColor * ambientLightStrength;
	float3 appliedLight = ambientLight;

	float3 vectorToLight = normalize(dynamicLightPosition - input.inWorldPos);
	// ������ �Ǵ°� �����ϱ� ���� max�Լ��� ������
	float3 diffuseLightIntensity = max(0, dot(vectorToLight, input.inNormal));
	float distanceToLight = distance(dynamicLightPosition, input.inWorldPos);
	float attenuationFactor = 1 / (dynamicLightAttenA + dynamicLightAttenB * distanceToLight + dynamicLightAttenC * pow(distanceToLight, 2));
	diffuseLightIntensity *= attenuationFactor;
	float3 diffuseLight = diffuseLightIntensity * dynamicLightStrength * dynamicLightColor;

	appliedLight += diffuseLight;
	float3 finalColor = sampleColor * appliedLight;
	return float4(finalColor, 1.0f);
}