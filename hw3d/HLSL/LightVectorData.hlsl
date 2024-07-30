struct LightVectorData
{
    float3 viewFragToLight; // ���㵽���Դ������
    float distFragToLight; // ��������
    float3 directionToLight; // ���㵽���Դ�ķ���
};

LightVectorData CalculateLightVectorData(const in float3 lightPos, const in float3 fragPos)
{
    LightVectorData lightVector;
    // ���㵽���Դ������
    lightVector.viewFragToLight = lightPos - fragPos;
    lightVector.distFragToLight = length(lightVector.viewFragToLight);
	// ���㵽���Դ�ķ���
    lightVector.directionToLight = lightVector.viewFragToLight / lightVector.distFragToLight;
    return lightVector;
}