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
	// "SV_POSITION"에서의 "SV"는 system value 라고 함, 다른 쉐이더로 넘길 값을 뜻하는 듯? 얘는 position값을 픽셀 쉐이더로 넘기니까 "SV_POSITION"으로 쓴듯
	float4 inPosition:SV_POSITION;
	float2 inTexCoord:TEXCOORD;
	float3 inNormal:NORMAL;
	float3 inWorldPos:WORLD_POSITION;
};

// Texture: 이미지
// Sampler: 이미지를 불러오는 방법(인덱스 벗어났을 때 어찌할지, 보간 방법 등)
Texture2D objTexture : TEXTURE: register(t0);
SamplerState objSamplerState : SAMPLER: register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
	//float3 sampleColor = input.inNormal;
	float3 ambientLight = ambientLightColor * ambientLightStrength;
	float3 appliedLight = ambientLight;

	float3 vectorToLight = normalize(dynamicLightPosition - input.inWorldPos);
	// 음수가 되는걸 방지하기 위해 max함수를 적용함
	float3 diffuseLightIntensity = max(0, dot(vectorToLight, input.inNormal));
	float distanceToLight = distance(dynamicLightPosition, input.inWorldPos);
	float attenuationFactor = 1 / (dynamicLightAttenA + dynamicLightAttenB * distanceToLight + dynamicLightAttenC * pow(distanceToLight, 2));
	diffuseLightIntensity *= attenuationFactor;
	float3 diffuseLight = diffuseLightIntensity * dynamicLightStrength * dynamicLightColor;

	appliedLight += diffuseLight;
	float3 finalColor = sampleColor * appliedLight;
	return float4(finalColor, 1.0f);
}