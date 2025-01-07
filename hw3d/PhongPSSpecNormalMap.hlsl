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

Texture2D tex; // 漫反射贴图
Texture2D specTex; // 镜面高光贴图
Texture2D normalMap; // 法线贴图

SamplerState splr; // 采样器


float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 viewTangent : Tangent, float3 viewBitangent : Bitangent, float2 tc : Texcoord) : SV_TARGET
{
    // do alpha test    
    float4 dtex = tex.Sample(splr, tc);
    clip(dtex.a < 0.1f ? -1 : 1);
    
#ifdef MASK_BOI
    // 反转背面三角的法线方向
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
    
	//// 顶点到点光源的向量
 //   const float3 viewFragToLight = viewLightPos - viewPos;
 //   const float distFragToLight = length(viewFragToLight);
	//// 顶点到点光源的方向
 //   const float3 directionToLight = viewFragToLight / distFragToLight;

    float3 specularReflectionColor;
    float specularPower = specularPowerConst;
    if (specularMapEnabled)
    {
        // 高光贴图采样
        const float4 specularSample = specTex.Sample(splr, tc);
        specularReflectionColor = specularSample.rgb * specularMapWeight; // 高光强度
        
        if (hasGloss)
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    }
    else
    {
        // 没有高光贴图则用颜色常量
        specularReflectionColor = specularColor;
    }
    
    // 漫反射衰减公式 diffuse attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lightVectorData.distFragToLight);
	// 漫反射颜色
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lightVectorData.directionToLight, viewNormal);
    
    const float3 specularReflected = Speculate(specularReflectionColor, 1.0f, viewNormal, lightVectorData.viewFragToLight, viewPos, att, specularPower);

	// 最终颜色 漫反射+环境光
    return float4(saturate((diffuse + ambient) * dtex.rgb + specularReflected), dtex.a);
}