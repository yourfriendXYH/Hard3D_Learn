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
    float padding[3];
};

Texture2D tex; // 漫反射贴图
Texture2D specTex; // 镜面高光贴图
Texture2D normalMap; // 法线贴图

SamplerState splr; // 采样器

float4 main(float3 worldPos : Position, float3 normal : Normal, float3 tangent : Tangent, float3 bitangent : Bitangent, float2 tc : Texcoord) : SV_TARGET
{
    if (normalMapEnabled)
    {
        // build the tranform (rotation) into tangent space
        const float3x3 tanToView = float3x3(
            normalize(tangent),
            normalize(bitangent),
            normalize(normal)
        );
        // unpack normal data
        const float3 normalSample = normalMap.Sample(splr, tc).xyz;
        normal.x = normalSample.x * 2.0f - 1.0f;
        normal.y = -normalSample.y * 2.0f + 1.0f;
        normal.z = normalSample.z;
        // bring normal from tanspace into view space
        normal = mul(normal, tanToView);
    }
    
	// 顶点到点光源的向量
    const float3 vectorToLight = lightPos - worldPos;
    const float distVectorToLight = length(vectorToLight);
	// 顶点到点光源的方向
    const float3 directionToLight = vectorToLight / distVectorToLight;
	
	// 漫反射衰减公式 diffuse attenuation
    const float att = 1 / (attQuad * (distVectorToLight * distVectorToLight) + attLin * distVectorToLight + attConst);
	// 漫反射颜色
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(directionToLight, normal));

	// 镜面高光计算
    const float3 w = normal * dot(vectorToLight, normal);
    const float3 r = 2.0f * w - vectorToLight; // 反射光R的计算
	// 镜面高光公式
	//const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
    const float4 specularSample = specTex.Sample(splr, tc);
    const float3 specularReflectionColor = specularSample.rgb; // 高光强度
    const float specularPower = pow(2.0f, specularSample.a * 13.0f);
    const float3 specular = att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);

	// 最终颜色 漫反射+环境光
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular * specularReflectionColor), 1.0f);
}