#version 300 es
// Vertex information
layout(location = 0) in vec3  VertexPosition;
layout(location = 1) in vec2  TexCoord;

out vec2 TextureCoordinates;

void main() 
{
    // 텍스처 코디네이트?
    TextureCoordinates = TexCoord;
    gl_Position = vec4(VertexPosition, 1.);
}