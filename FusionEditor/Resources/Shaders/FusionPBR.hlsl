cbuffer TransformData
{
	float4x4 TransformMatrix;
	uint2 EnttID;
};

cbuffer CameraData
{
	float4x4 ViewProjectionMatrix;
};

struct VertexInput
{
    float4 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct PixelInput
{
    float4 Position : SV_Position;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

PixelInput VertexMain(VertexInput InVertexData)
{
	PixelInput Output;
	
	InVertexData.Position.w = 1.0f;

	float4 WorldPosition = mul(TransformMatrix, InVertexData.Position);
	Output.Position = mul(ViewProjectionMatrix, WorldPosition);
	Output.Normal = InVertexData.Normal;
	Output.TexCoord = InVertexData.TexCoord;
	
	return Output;
}

Texture2D InTexture;
SamplerState InSampler;

struct PixelOutput
{
	float4 Color : COLOR0;
	uint2 EnttID : COLOR1;
};

PixelOutput PixelMain(PixelInput InPixelData) : SV_Target
{
	PixelOutput Output;
	Output.Color = float4(InPixelData.Normal, 1.0f);
	Output.EnttID = EnttID;
	//return InTexture.Sample(InSampler, InPixelData.TexCoord);
	return Output;
}
