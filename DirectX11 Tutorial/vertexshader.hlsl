cbuffer mycBuffer : register(b0)// b0이 첫번째 Buffer Slot을 사용한다는 얘기임
{
	float4x4 mat;
	//row_major float4x4 mat;
};

struct VS_INPUT
{
// 여기서 "POSITION" 이건 Graphics::InitializeShaders()에서 layout에 정해둔 이름이랑만 맞으면 되는듯
	float3 inPos:POSITION;
	float2 inTexCoord:TEXCOORD;
};

struct VS_OUTPUT
{
// "SV_POSITION"에서의 "SV"는 system value 라고 함, 다른 쉐이더로 넘길 값을 뜻하는 듯? 얘는 position값을 픽셀 쉐이더로 넘기니까 "SV_POSITION"으로 쓴듯
	float4 outPosition:SV_POSITION;
	float2 outTexCoord:TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.outPosition = mul(float4(input.inPos, 1.0f), mat);
	output.outTexCoord = input.inTexCoord;
	return output;
}