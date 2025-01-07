#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"
#include "PointLight.hlsl" // 点光源的常数缓存

cbuffer ObjectCBuf
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex; // 纹理数据的常数缓存

SamplerState splr; // 纹理采样器

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : TexCoord) : SV_TARGET
{
    // renormalize interpolated normal
    viewNormal = normalize(viewNormal);
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distFragToLight);
	// diffuse
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.directionToLight, viewNormal);
	// specular
    const float3 specular = Speculate(diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal, lv.viewFragToLight, viewFragPos, att, specularGloss);
	// final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}