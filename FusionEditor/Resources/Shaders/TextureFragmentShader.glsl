#version 450

layout(location = 0) in vec2 PassTextureCoords;

layout(location = 0) out vec4 OutColor;

uniform sampler2D MainTexture;

void main()
{
    OutColor = texture2D(MainTexture, PassTextureCoords);
}
