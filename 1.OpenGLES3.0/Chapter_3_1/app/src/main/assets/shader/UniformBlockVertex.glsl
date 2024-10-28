#version 300 es

layout(location = 0) in vec4 VertexPosition;
layout(location = 1) in vec4 VertexColor;

out vec4 Color;

//
layout(std140) uniform Transformation{
    mat4 ModelMatrix;
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
};

// Uniform Block Declaration
uniform mat4 MODELVIEWPROJECTIONMATRIX;

void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * VertexPosition;
    //gl_Position = MODELVIEWPROJECTIONMATRIX * VertexPosition;
    Color = VertexColor;
}