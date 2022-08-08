// 전구(Light) 그 자체는 빛의 영향 없이 항상 보여야 하니까 걔네들을 위해서 쉐이더를 따로 만듬

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
	return float4(sampleColor, 1.0f);
}