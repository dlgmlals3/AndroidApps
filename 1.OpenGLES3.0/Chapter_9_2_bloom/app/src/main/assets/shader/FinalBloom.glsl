#version 300 es
precision mediump float;

in vec2 TexCoord; // 텍스처 좌표 입력 (버텍스 셰이더에서 전달)
uniform sampler2D sceneTexture; // 원본 씬 텍스처
uniform sampler2D bloomTexture; // 블러된 Bloom 텍스처
uniform float bloomIntensity;   // Bloom 효과 강도

out vec4 FragColor; // 최종 출력 색상

void main() {
    // 원본 씬 텍스처와 Bloom 텍스처 샘플링
    vec4 sceneColor = texture(sceneTexture, TexCoord);
    vec4 bloomColor = texture(bloomTexture, TexCoord);

    // Bloom 텍스처를 원본 텍스처와 결합
    vec4 finalColor = sceneColor + bloomIntensity * bloomColor;
    //finalColor = bloomColor;
    // 최종 출력
    FragColor = vec4(finalColor.rgb, 1.0); // 알파는 항상 1.0
}