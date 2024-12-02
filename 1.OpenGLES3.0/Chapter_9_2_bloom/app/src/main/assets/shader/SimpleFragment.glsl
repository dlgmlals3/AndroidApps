#version 300 es

precision mediump float;

in vec2 TextureCoordinates;
uniform sampler2D simpleTexture;

out vec4 FragColor;

void main() {
    FragColor = texture(simpleTexture, TextureCoordinates);
    //FragColor = vec4(1.0, 1.0, 0.0, 1.0); // 빨간색
}