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

cbuffer ObjectCBuf
{
    bool normalMapEnabled;
    bool specularMapEnabled;
    bool hasGloss;
    float specularPowerConst;
    float3 specularColor;
    float specularMapWeight;
};

Texture2D tex; // ��������ͼ
Texture2D specTex; // ����߹���ͼ
Texture2D normalMap; // ������ͼ

SamplerState splr; // ������

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
	//const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);

    float3 specularReflectionColor;
    float specularPower = specularPowerConst;
    if (specularMapEnabled)
    {
        // �߹���ͼ����
        const float4 specularSample = specTex.Sample(splr, tc);
        specularReflectionColor = specularSample.rgb * specularMapWeight; // �߹�ǿ��
        
        if (hasGloss)
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    }
    else
    {
        // û�и߹���ͼ������ɫ����
        specularReflectionColor = specularColor;
    }
    
    const float3 specular = att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);

	// ������ɫ ������+������
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular * specularReflectionColor), 1.0f);
}