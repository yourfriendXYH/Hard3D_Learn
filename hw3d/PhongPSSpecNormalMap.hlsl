cbuffer LightCBuf
{
    float3 viewLightPos; // 点光源位置
    float3 ambient; // 环境光颜色
    float3 diffuseColor; // 漫反射颜色
    float diffuseIntensity; // 漫反射强度
    float attQuad = 0.0075f; // 点光源光照强度的3个常量
    float attLin = 0.045f;
    float attConst = 1.0f;
};

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

// 将法线方向乘上切线空间的矩阵
float3 MapNormalViewSpace(const float3 tangent, const float3 bitangent, const float3 normal, const float2 tc, Texture2D normalMap, SamplerState splr)
{
    const float3x3 tanToTarget = float3x3(tangent, bitangent, normal);
        // unpack normal data
    const float3 normalSample = normalMap.Sample(splr, tc).xyz;
    const float3 tanNormal = normalSample * 2.0f - 1.0f;
    
    return normalize(mul(tanNormal, tanToTarget));
}

// 点光源光强衰减
float Attenuate(uniform float attConst, uniform float attLin, uniform float attQuad, const in float distFragToLight)
{
    return 1 / (attQuad * (distFragToLight * distFragToLight) + attLin * distFragToLight + attConst);
}

// 漫反射颜色
float3 Diffuse(uniform float3 diffuseColor, // 漫反射颜色  
                uniform float diffuseIntensity, // 漫反射强度
                const in float att, // 光强衰减
                const in float3 viewDirFragToLight, // 模型顶点到点光源的方向
                const in float3 viewNormal) // 模型顶点的法线方向
{
    return diffuseColor * diffuseIntensity * att * max(0.0f, dot(viewDirFragToLight, viewNormal));
}

// 高光计算
float3 Speculate(const in float3 specularColor,
                    uniform float specularIntensity,
                    const in float3 viewNormal,
                    const in float3 viewFragToLight,
                    const in float3 viewPos,
                    const in float att,
                    const in float specularPower)
{
    	// 镜面高光计算
    const float3 w = viewNormal * dot(viewFragToLight, viewNormal);
    const float3 r = 2.0f * w - viewFragToLight; // 反射光R的计算
    
    return att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
}

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 viewTangent : Tangent, float3 viewBitangent : Bitangent, float2 tc : Texcoord) : SV_TARGET
{
    viewNormal = normalize(viewNormal);
    if (normalMapEnabled)
    {
        viewNormal = MapNormalViewSpace(normalize(viewTangent), normalize(viewBitangent), viewNormal, tc, normalMap, splr);
    }
    
	// 顶点到点光源的向量
    const float3 viewFragToLight = viewLightPos - viewPos;
    const float distFragToLight = length(viewFragToLight);
	// 顶点到点光源的方向
    const float3 directionToLight = viewFragToLight / distFragToLight;

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
    const float att = Attenuate(attConst, attLin, attQuad, distFragToLight);
	// 漫反射颜色
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, directionToLight, viewNormal);
    
    const float3 specularReflected = Speculate(specularReflectionColor, 1.0f, viewNormal, viewFragToLight, viewPos, att, specularPower);

	// 最终颜色 漫反射+环境光
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specularReflected), 1.0f);
}