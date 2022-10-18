cbuffer TransformData
{
	float4x4 TransformMatrix;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
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
	float4 ViewPosition = mul(ViewMatrix, WorldPosition);
	float4 ScreenPosition = mul(ProjectionMatrix, ViewPosition);
	Output.Position = ScreenPosition;
	
	//float4 WorldPosition = mul(InVertexData.Position, TransformMatrix);
	//float4 ViewPosition = mul(WorldPosition, ViewMatrix);
	//Output.Position = mul(ViewPosition, ProjectionMatrix);
	//Output.Position = InVertexData.Position;
	Output.Normal = InVertexData.Normal;
	Output.TexCoord = InVertexData.TexCoord;
	
	return Output;
}

float4 PixelMain(PixelInput InPixelData) : SV_Target
{
	return float4(InPixelData.Normal, 1.0f);
	//return InPixelData.Position;
}
