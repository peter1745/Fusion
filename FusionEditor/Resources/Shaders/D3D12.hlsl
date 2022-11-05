cbuffer TransformData
{
	float4x4 ViewProjection;
	float4x4 Transform;
	uint2 ActorID;
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

	float4 WorldPosition = mul(Transform, InVertexData.Position);
	Output.Position = mul(ViewProjection, WorldPosition);
	Output.Normal = InVertexData.Normal;
	Output.TexCoord = InVertexData.TexCoord;
	
	return Output;
}

struct PixelOutput
{
	float4 Color : COLOR0;
};

PixelOutput PixelMain(PixelInput InPixelData) : SV_Target
{
	PixelOutput Output;
	Output.Color = float4(InPixelData.Normal, 1.0f);
	return Output;
}
