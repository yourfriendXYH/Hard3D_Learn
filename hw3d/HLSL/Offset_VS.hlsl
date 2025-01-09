#include "Transform.hlsl"

// ʵ��ģ����ߵĶ�����ɫ��

cbuffer Offset : register(b1)
{
    float offset;
}

float4 main(float3 pos : Position, float3 normal : Normal) : SV_POSITION
{
    return mul(float4(pos + offset * normal, 1.0f), modelViewProj);
}