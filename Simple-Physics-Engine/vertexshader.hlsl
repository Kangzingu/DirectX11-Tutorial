#pragma pack_matrix( row_major )

cbuffer perObjectBuffer : register(b0)// b0이 첫번째 Buffer Slot을 사용한다는 얘기임
{
	float4x4 wvpMatrix;
	// D3D 에서는 Model Matrix가 아닌 World Matrix라고 불러서 WVP Matrix가 되는 듯
	float4x4 worldMatrix;
	//row_major float4x4 mat;
};

struct VS_INPUT
{
// 여기서 "POSITION" 이건 Renderer::InitializeShaders()에서 layout에 정해둔 이름이랑만 맞으면 되는듯
	float3 inPos:POSITION;
	float2 inTexCoord:TEXCOORD;
	float3 inNormal:NORMAL;
};

struct VS_OUTPUT
{
// "SV_POSITION"에서의 "SV"는 system value 라고 함, 다른 쉐이더로 넘길 값을 뜻하는 듯? 얘는 position값을 픽셀 쉐이더로 넘기니까 "SV_POSITION"으로 쓴듯
	float4 outPosition:SV_POSITION;
	float2 outTexCoord:TEXCOORD;
	float3 outNormal:NORMAL;
	float3 outWorldPos:WORLD_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix);
	output.outTexCoord = input.inTexCoord;
	output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), worldMatrix));
	output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);
	return output;
}