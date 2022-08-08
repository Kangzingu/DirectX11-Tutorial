// ����(Light) �� ��ü�� ���� ���� ���� �׻� ������ �ϴϱ� �³׵��� ���ؼ� ���̴��� ���� ����

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
	return float4(sampleColor, 1.0f);
}