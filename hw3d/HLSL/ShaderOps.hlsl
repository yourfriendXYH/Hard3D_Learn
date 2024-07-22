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
    
    return att * (specularColor * specularIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
}