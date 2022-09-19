#version 450

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec2 InTextureCoords;

layout(location = 0) out vec2 PassTextureCoords;

void main()
{
    gl_Position = vec4(InPosition, 1.0);
    PassTextureCoords = InTextureCoords;
}
