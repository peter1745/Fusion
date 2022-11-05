cbuffer TransformData
{
	float4x4 ViewProjectionMatrix;
	float4x4 TransformMatrix;
	uint2 ActorID;
};

struct VertexInput
{
    float3 Position : POSITION;
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

	float4 WorldPosition = mul(TransformMatrix, float4(InVertexData.Position, 1.0f));
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
	uint2 ActorID : COLOR1;
};

PixelOutput PixelMain(PixelInput InPixelData) : SV_Target
{
	PixelOutput Output;
	Output.Color = float4(InPixelData.Normal, 1.0f);
	Output.ActorID = ActorID;
	//return InTexture.Sample(InSampler, InPixelData.TexCoord);
	return Output;
}
