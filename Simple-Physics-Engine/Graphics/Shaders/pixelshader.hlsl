cbuffer lightBuffer : register(b0)
{
    float3 lightPosition;
    float lightStrength;
    float3 cameraPosition;
}
struct PS_INPUT
{
	float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPosition : WORLD_POSITION;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);
float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord) / 2.0f;
	
    float3 ambient = 0.3f;

    float3 diffuse;
    float3 objectToLight = normalize(lightPosition - input.inWorldPosition);
    float diffuseStrength = max(0, dot(objectToLight, input.inNormal));
    float objectToLightDistance = distance(lightPosition, input.inWorldPosition);
    float attenuation = 1.0f / (objectToLightDistance * objectToLightDistance);
    diffuse = diffuseStrength * attenuation;
    
    float3 specular = 0;
    //if (dot(objectToLight, input.inNormal) > 0)
    //{
        float3 objectToCamera = normalize(cameraPosition - input.inWorldPosition);
        float specularStrength = max(0, dot(normalize(objectToLight + objectToCamera), input.inNormal)) * 0.8f;
        specular = pow(specularStrength, 3);
    //}

    float3 finalColor = sampleColor + ambient + diffuse + specular;
    return float4(finalColor, 1.0f);
}