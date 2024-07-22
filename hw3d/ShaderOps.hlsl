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
    
    return att * (specularColor * specularIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
}