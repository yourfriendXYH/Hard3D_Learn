cbuffer LightCBuf
{
    float3 lightPos; // 点光源位置
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

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 tangent : Tangent, float3 bitangent : Bitangent, float2 tc : Texcoord) : SV_TARGET
{
    if (normalMapEnabled)
    {
        // build the tranform (rotation) into tangent space
        const float3x3 tanToView = float3x3(
            normalize(tangent),
            normalize(bitangent),
            normalize(viewNormal)
        );
        // unpack normal data
        const float3 normalSample = normalMap.Sample(splr, tc).xyz;
        viewNormal.x = normalSample.x * 2.0f - 1.0f;
        viewNormal.y = -normalSample.y * 2.0f + 1.0f;
        viewNormal.z = normalSample.z;
        // bring normal from tanspace into view space
        viewNormal = mul(viewNormal, tanToView);
    }
    
	// 顶点到点光源的向量
    const float3 vectorToLight = lightPos - viewPos;
    const float distVectorToLight = length(vectorToLight);
	// 顶点到点光源的方向
    const float3 directionToLight = vectorToLight / distVectorToLight;
	
	// 漫反射衰减公式 diffuse attenuation
    const float att = 1 / (attQuad * (distVectorToLight * distVectorToLight) + attLin * distVectorToLight + attConst);
	// 漫反射颜色
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(directionToLight, viewNormal));

	// 镜面高光计算
    const float3 w = viewNormal * dot(vectorToLight, viewNormal);
    const float3 r = 2.0f * w - vectorToLight; // 反射光R的计算
	// 镜面高光公式
	//const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);

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
    
    const float3 specular = att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);

	// 最终颜色 漫反射+环境光
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular * specularReflectionColor), 1.0f);
}