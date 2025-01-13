Texture2D tex;
SamplerState splr;

static const int r = 12;
static const float divisor = (2 * r + 1) * (2 * r + 1);

float4 main(float2 uv : Texcoord) : SV_TARGET
{
    float4 acc = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float accAlpha = 0.0f;
    float3 maxColor = float3(0.0f, 0.0f, 0.0f);

    //
    //return 1.0f - tex.Sample(splr, uv).rgba;
    
    // 实现模糊效果
    uint width;
    uint height;
    tex.GetDimensions(width, height);
    const float dx = 1.0f / width;
    const float dy = 1.0f / height;
    for (int y = -r; y <= r; y++)
    {
        for (int x = -r; x <= r; x++)
        {
            const float2 tc = uv + float2(dx * x, dy * y);
            //acc += tex.Sample(splr, tc).rgba;
            const float4 s = tex.Sample(splr, tc).rgba;
            accAlpha += s.a;
            maxColor = max(s.rgb, maxColor);
        }
    }
    return float4(maxColor, accAlpha / divisor);

    //return tex.Sample(splr, uv).rgba;
}