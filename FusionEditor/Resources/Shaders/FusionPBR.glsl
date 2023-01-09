#version 450

#ifdef FUSION_COMPILE_VERTEX

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 InTexCoord;

layout(push_constant) uniform TransformData
{
    mat4x4 ViewProjectionMatrix;
    mat4x4 TransformMatrix;
    uvec2 ActorID;
} InTransformData;

struct VertexOutput
{
    vec3 Normal;
    vec2 TexCoord;
};

layout(location = 0) out VertexOutput Out;

void main()
{
    gl_Position = InTransformData.ViewProjectionMatrix * InTransformData.TransformMatrix * vec4(InPosition, 1.0);
    Out.Normal = InNormal;
    Out.TexCoord = InTexCoord;
}

#endif

#ifdef FUSION_COMPILE_PIXEL

struct PixelInput
{
    vec3 Normal;
    vec2 TexCoord;
};

layout(location = 0) in PixelInput InData;

layout(location = 0) out vec4 OutColor;

void main()
{
    //vec3 NormalColor = (normalize(InData.Normal) * 0.5) + 0.5;
    OutColor = vec4(1.0, 0.0, 0.0, 1.0);
}

#endif
