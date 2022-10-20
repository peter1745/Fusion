cbuffer CameraData
{
	float4x4 ViewProjectionMatrix;
};

cbuffer TransformData
{
	float4x4 TransformMatrix;
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

float4 PixelMain(PixelInput InPixelData) : SV_Target
{
	return InTexture.Sample(InSampler, InPixelData.TexCoord);
}
