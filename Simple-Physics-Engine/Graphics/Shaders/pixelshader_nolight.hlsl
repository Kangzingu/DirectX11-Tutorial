struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPosition : WORLD_POSITION;
};

Texture2D objectTexture : TEXTURE : register(t0);
SamplerState objectSamplerState : SAMPLER : register(s0);
float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampleColor = objectTexture.Sample(objectSamplerState, input.inTexCoord);
    return float4(sampleColor, 1.0f);
}