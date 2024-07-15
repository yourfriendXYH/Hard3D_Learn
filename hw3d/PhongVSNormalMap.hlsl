cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float3 tangent : Tangent;
    float3 bitangent : Bitangent;
    float2 tc : Texcoord;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 normal : Normal, float3 tangent : Tangent, float3 bitangent : Bitangent, float2 tc : Texcoord)
{
    VSOut vso;
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNormal = mul(normal, (float3x3) modelView);
    vso.tangent = mul(tangent, (float3x3) modelView);
    vso.bitangent = mul(bitangent, (float3x3) modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj); // 要乘上投影矩阵
    vso.tc = tc;
    
    return vso;
}