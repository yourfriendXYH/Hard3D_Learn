#include "ShaderOps.hlsl"
#include "HLSL/PointLight.hlsl"
#include "HLSL/LightVectorData.hlsl"

cbuffer ObjectCBuf
{
    bool normalMapEnabled;
    bool specularMapEnabled;
    bool hasGloss;
    float specularPowerConst;
    float3 specularColor;
    float specularMapWeight;
};

Texture2D tex; // ��������ͼ
Texture2D specTex; // ����߹���ͼ
Texture2D normalMap; // ������ͼ

SamplerState splr; // ������


float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 viewTangent : Tangent, float3 viewBitangent : Bitangent, float2 tc : Texcoord) : SV_TARGET
{
    // do alpha test    
    float4 dtex = tex.Sample(splr, tc);
    clip(dtex.a < 0.1f ? -1 : 1);
    
#ifdef MASK_BOI
    // ��ת�������ǵķ��߷���
    if (dot(viewPos, viewNormal) >= 0.0f)
    {
        viewNormal = -viewNormal;
    }
    
    viewNormal = normalize(viewNormal);
    if (normalMapEnabled)
    {
        viewNormal = MapNormalViewSpace(normalize(viewTangent), normalize(viewBitangent), viewNormal, tc, normalMap, splr);
    }
#endif
    
    const LightVectorData lightVectorData = CalculateLightVectorData(viewLightPos, viewPos);
    
	//// ���㵽���Դ������
 //   const float3 viewFragToLight = viewLightPos - viewPos;
 //   const float distFragToLight = length(viewFragToLight);
	//// ���㵽���Դ�ķ���
 //   const float3 directionToLight = viewFragToLight / distFragToLight;

    float3 specularReflectionColor;
    float specularPower = specularPowerConst;
    if (specularMapEnabled)
    {
        // �߹���ͼ����
        const float4 specularSample = specTex.Sample(splr, tc);
        specularReflectionColor = specularSample.rgb * specularMapWeight; // �߹�ǿ��
        
        if (hasGloss)
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    }
    else
    {
        // û�и߹���ͼ������ɫ����
        specularReflectionColor = specularColor;
    }
    
    // ������˥����ʽ diffuse attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lightVectorData.distFragToLight);
	// ��������ɫ
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lightVectorData.directionToLight, viewNormal);
    
    const float3 specularReflected = Speculate(specularReflectionColor, 1.0f, viewNormal, lightVectorData.viewFragToLight, viewPos, att, specularPower);

	// ������ɫ ������+������
    return float4(saturate((diffuse + ambient) * dtex.rgb + specularReflected), dtex.a);
}