#type vertex
#version 450

layout(location = 0) in vec3 InVertexPos;
layout(location = 1) in vec3 InVertexNormal;
layout(location = 2) in vec2 InVertexUV;

layout(location = 0) out vec3 PassVertexNormal;
layout(location = 1) out vec2 PassVertexUV;

void main()
{
    gl_Position = vec4(InVertexPos, 1.0);
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
    OutColor = vec4(PassVertexUV.x, PassVertexUV.y, PassVertexNormal.z, 1.0);
}
