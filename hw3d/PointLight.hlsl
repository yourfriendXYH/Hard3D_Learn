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