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

// TestCube给的常量缓存
cbuffer ObjectCbuf
{
    float specularIntensity; // 镜面高光强度
    float specularPower; // 镜面高光功率
    float padding[2];
};

Texture2D tex;
SamplerState splr;

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_TARGET
{
    // 法线方向必须规范化
    viewNormal = normalize(viewNormal);
    
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
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);

	// 最终颜色 漫反射+环境光
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}