

cbuffer CBuf
{
    matrix transform;
};

// new
float4 main(float3 pos : Position) : SV_Position
{
    //VSOut vsout;
    ////vsout.color = color(1.f, 1.f, 1.f);
    //vsout.pos = mul(float4(pos.x, pos.y, pos.z, 1.f), transform);
    //return vsout;
    return mul(float4(pos, 1.f), transform);
}