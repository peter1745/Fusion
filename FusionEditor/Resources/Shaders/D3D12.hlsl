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

	Output.Position = InVertexData.Position;
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
	Output.Color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	return Output;
}
