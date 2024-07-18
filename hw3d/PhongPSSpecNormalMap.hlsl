cbuffer LightCBuf
{
    float3 viewLightPos; // ���Դλ��
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

// �����߷���������߿ռ�ľ���
float3 MapNormalViewSpace(const float3 tangent, const float3 bitangent, const float3 normal, const float2 tc, Texture2D normalMap, SamplerState splr)
{
    const float3x3 tanToTarget = float3x3(tangent, bitangent, normal);
        // unpack normal data
    const float3 normalSample = normalMap.Sample(splr, tc).xyz;
    const float3 tanNormal = normalSample * 2.0f - 1.0f;
    
    return normalize(mul(tanNormal, tanToTarget));
}

// ���Դ��ǿ˥��
float Attenuate(uniform float attConst, uniform float attLin, uniform float attQuad, const in float distFragToLight)
{
    return 1 / (attQuad * (distFragToLight * distFragToLight) + attLin * distFragToLight + attConst);
}

// ��������ɫ
float3 Diffuse(uniform float3 diffuseColor, // ��������ɫ  
                uniform float diffuseIntensity, // ������ǿ��
                const in float att, // ��ǿ˥��
                const in float3 viewDirFragToLight, // ģ�Ͷ��㵽���Դ�ķ���
                const in float3 viewNormal) // ģ�Ͷ���ķ��߷���
{
    return diffuseColor * diffuseIntensity * att * max(0.0f, dot(viewDirFragToLight, viewNormal));
}

// �߹����
float3 Speculate(const in float3 specularColor,
                    uniform float specularIntensity,
                    const in float3 viewNormal,
                    const in float3 viewFragToLight,
                    const in float3 viewPos,
                    const in float att,
                    const in float specularPower)
{
    	// ����߹����
    const float3 w = viewNormal * dot(viewFragToLight, viewNormal);
    const float3 r = 2.0f * w - viewFragToLight; // �����R�ļ���
    
    return att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
}

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 viewTangent : Tangent, float3 viewBitangent : Bitangent, float2 tc : Texcoord) : SV_TARGET
{
    viewNormal = normalize(viewNormal);
    if (normalMapEnabled)
    {
        viewNormal = MapNormalViewSpace(normalize(viewTangent), normalize(viewBitangent), viewNormal, tc, normalMap, splr);
    }
    
	// ���㵽���Դ������
    const float3 viewFragToLight = viewLightPos - viewPos;
    const float distFragToLight = length(viewFragToLight);
	// ���㵽���Դ�ķ���
    const float3 directionToLight = viewFragToLight / distFragToLight;

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
    
    // ������˥����ʽ diffuse attenuation
    const float att = Attenuate(attConst, attLin, attQuad, distFragToLight);
	// ��������ɫ
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, directionToLight, viewNormal);
    
    const float3 specularReflected = Speculate(specularReflectionColor, 1.0f, viewNormal, viewFragToLight, viewPos, att, specularPower);

	// ������ɫ ������+������
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specularReflected), 1.0f);
}