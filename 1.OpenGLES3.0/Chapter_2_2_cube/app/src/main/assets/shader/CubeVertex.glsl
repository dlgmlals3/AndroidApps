#version 300 es

uniform mat4 MODELVIEWPROJECTIONMATRIX;
in vec3 VertexPosition;
in vec3 VertexColor;
out vec4 VarColor;

void main()
{
    gl_Position = MODELVIEWPROJECTIONMATRIX * vec4(VertexPosition,1.0);
    VarColor    = vec4(VertexColor, 1.0);
}