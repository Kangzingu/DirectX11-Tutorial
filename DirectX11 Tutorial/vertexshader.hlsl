struct VS_INPUT
{
// ���⼭ "POSITION" �̰� Graphics::InitializeShaders()���� layout�� ���ص� �̸��̶��� ������ �Ǵµ�
	float3 inPos:POSITION;
	float2 inTexCoord:TEXCOORD;
};

struct VS_OUTPUT
{
// "SV_POSITION"������ "SV"�� system value ��� ��, �ٸ� ���̴��� �ѱ� ���� ���ϴ� ��? ��� position���� �ȼ� ���̴��� �ѱ�ϱ� "SV_POSITION"���� ����
	float4 outPosition:SV_POSITION;
	float2 outTexCoord:TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.outPosition = float4(input.inPos, 1.0f);
	output.outTexCoord = input.inTexCoord;
	return output;
}