#pragma pack_matrix( column_major )

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
};
struct VS_INPUT
{
// Engine::InitializeShaders() ¿¡¼­ layout¿¡ Á¤ÇØµÐ ÀÌ¸§ÀÌ¶û ¸ÂÃç¾ßÇÔ
    float3 inPosition : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};
struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPosition : WORLD_POSITION;
};
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPosition, 1.0f), wvpMatrix);
    output.outTexCoord = input.inTexCoord;
    output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), worldMatrix));
    output.outWorldPosition = mul(float4(input.inPosition, 1.0f), worldMatrix);
    return output;
}