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
layout(location = 2) out flat uvec2 OutActorID;

void main()
{
    gl_Position = InTransformData.ViewProjectionMatrix * InTransformData.TransformMatrix * vec4(InPosition, 1.0);
    Out.Normal = InNormal;
    Out.TexCoord = InTexCoord;
    OutActorID = InTransformData.ActorID;
}

#endif

#ifdef FUSION_COMPILE_PIXEL

struct PixelInput
{
    vec3 Normal;
    vec2 TexCoord;
};

layout(location = 0) in PixelInput InData;
layout(location = 2) in flat uvec2 InActorID;

layout(location = 0) out vec4 OutColor;
layout(location = 1) out uvec2 OutActorID;

// Material Data
layout(set = 0, binding = 0) uniform sampler2D InTexture;

// Lighting Data
layout(set = 1, binding = 0) uniform LightData
{
    vec4 Color;
} InLightData;

const float c_AmbientStrength = 1.0;

void main()
{
    vec4 AmbientLighting = InLightData.Color * c_AmbientStrength;
    OutColor = AmbientLighting * texture(InTexture, InData.TexCoord);
    OutActorID = InActorID;
}

#endif
