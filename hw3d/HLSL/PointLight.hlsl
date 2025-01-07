cbuffer LightCBuf
{
    float3 viewLightPos; // 点光源位置
    float3 ambient; // 环境光颜色
    float3 diffuseColor; // 漫反射颜色
    float diffuseIntensity; // 漫反射强度
    float attQuad = 0.0075f; // 点光源光照强度的3个常量
    float attLin = 0.045f;
    float attConst = 1.0f;
};