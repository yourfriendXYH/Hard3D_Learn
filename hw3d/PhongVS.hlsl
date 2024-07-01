cbuffer CBuf
{
	matrix modelView;
	matrix modelViewProj;
};

struct VSOut
{
	float3 worldPos : Position; 
	float3 normal : Normal;
	float2 textureCoord : Texcoord;
	float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 normal : Normal, float2 texcoord : Texcoord)
{
	VSOut vsOut;
	vsOut.worldPos = (float3)mul(float4(pos, 1.0f), modelView); // 模型的世界位置变换
	vsOut.normal = mul(normal, (float3x3)modelView); // 顶点的法线方向变换
	vsOut.pos = mul(float4(pos, 1.0f), modelViewProj);
	vsOut.textureCoord = texcoord;
	return vsOut;
}