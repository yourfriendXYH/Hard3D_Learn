struct LightVectorData
{
    float3 viewFragToLight; // 顶点到点光源的向量
    float distFragToLight; // 向量长度
    float3 directionToLight; // 顶点到点光源的方向
};

LightVectorData CalculateLightVectorData(const in float3 lightPos, const in float3 fragPos)
{
    LightVectorData lightVector;
    // 顶点到点光源的向量
    lightVector.viewFragToLight = lightPos - fragPos;
    lightVector.distFragToLight = length(lightVector.viewFragToLight);
	// 顶点到点光源的方向
    lightVector.directionToLight = lightVector.viewFragToLight / lightVector.distFragToLight;
    return lightVector;
}