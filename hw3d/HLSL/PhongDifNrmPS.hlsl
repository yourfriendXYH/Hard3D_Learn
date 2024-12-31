
cbuffer ObjectCBuf
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}