#pragma pack_matrix( row_major )

cbuffer perObjectBuffer : register(b0)// b0�� ù��° Buffer Slot�� ����Ѵٴ� �����
{
	float4x4 wvpMatrix;
	// D3D ������ Model Matrix�� �ƴ� World Matrix��� �ҷ��� WVP Matrix�� �Ǵ� ��
	float4x4 worldMatrix;
	//row_major float4x4 mat;
};

struct VS_INPUT
{
// ���⼭ "POSITION" �̰� Renderer::InitializeShaders()���� layout�� ���ص� �̸��̶��� ������ �Ǵµ�
	float3 inPos:POSITION;
	float2 inTexCoord:TEXCOORD;
	float3 inNormal:NORMAL;
};

struct VS_OUTPUT
{
// "SV_POSITION"������ "SV"�� system value ��� ��, �ٸ� ���̴��� �ѱ� ���� ���ϴ� ��? ��� position���� �ȼ� ���̴��� �ѱ�ϱ� "SV_POSITION"���� ����
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