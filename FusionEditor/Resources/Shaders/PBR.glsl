#type vertex
#version 450

layout(location = 0) in vec3 InVertexPos;
layout(location = 1) in vec3 InVertexNormal;
layout(location = 2) in vec2 InVertexUV;

uniform mat4 InProjectionMatrix;
uniform mat4 InViewMatrix = mat4(1.0);
uniform mat4 InTransformMatrix = mat4(1.0);

layout(location = 0) out vec3 PassVertexNormal;
layout(location = 1) out vec2 PassVertexUV;

void main()
{
    gl_Position = InProjectionMatrix * InViewMatrix * InTransformMatrix * vec4(InVertexPos, 1.0);
    PassVertexNormal = InVertexNormal;
    PassVertexUV = InVertexUV;
}

#type fragment
#version 450

layout(location = 0) in vec3 PassVertexNormal;
layout(location = 1) in vec2 PassVertexUV;

layout(location = 0) out vec4 OutColor;

void main()
{
    vec3 NormalColor = (normalize(PassVertexNormal) * 0.5) + 0.5;
    OutColor = vec4(NormalColor, 1.0);
}
