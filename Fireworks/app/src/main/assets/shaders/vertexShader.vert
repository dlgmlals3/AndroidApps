#version 310 es

layout(location = 0) in vec4 a_Position;
uniform mat4 uVPMatrix;
//attribute vec4 a_Position;

void main(void)
{
    gl_Position =  uVPMatrix * a_Position;
    gl_PointSize = 120.0;
}