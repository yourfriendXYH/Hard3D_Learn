#include "Transform.hlsl"

// 实现模型描边的顶点着色器

cbuffer Offset : register(b1)
{
    float offset;
}

float4 main(float3 pos : Position, float3 normal : Normal) : SV_POSITION
{
    return mul(float4(pos + offset * normal, 1.0f), modelViewProj);
}