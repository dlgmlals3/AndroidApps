#version 310 es

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec2 aTexCoordinate;

out vec2 v_TexCoord;
//uniform mat4 uVPMatrix;

void main(void)
{
    v_TexCoord = aTexCoordinate;
    gl_Position = vPosition;
}