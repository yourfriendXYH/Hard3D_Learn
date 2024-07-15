cbuffer LightCBuf
{
    float3 lightPos; // ���Դλ��
    float3 ambient; // ��������ɫ
    float3 diffuseColor; // ��������ɫ
    float diffuseIntensity; // ������ǿ��
    float attQuad = 0.0075f; // ���Դ����ǿ�ȵ�3������
    float attLin = 0.045f;
    float attConst = 1.0f;
};

// TestCube���ĳ�������
cbuffer ObjectCbuf
{
    float specularIntensity; // ����߹�ǿ��
    float specularPower; // ����߹⹦��
    float padding[2];
};

Texture2D tex;
SamplerState splr;

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_TARGET
{
    // ���߷������淶��
    viewNormal = normalize(viewNormal);
    
	// ���㵽���Դ������
    const float3 vectorToLight = lightPos - viewPos;
    const float distVectorToLight = length(vectorToLight);
	// ���㵽���Դ�ķ���
    const float3 directionToLight = vectorToLight / distVectorToLight;
	
	// ������˥����ʽ diffuse attenuation
    const float att = 1 / (attQuad * (distVectorToLight * distVectorToLight) + attLin * distVectorToLight + attConst);
	// ��������ɫ
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(directionToLight, viewNormal));

	// ����߹����
    const float3 w = viewNormal * dot(vectorToLight, viewNormal);
    const float3 r = 2.0f * w - vectorToLight; // �����R�ļ���
	// ����߹⹫ʽ
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);

	// ������ɫ ������+������
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}