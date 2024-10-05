#version 310 es

precision highp float;

in vec2 v_TexCoord;
out vec4 fragColor;

uniform sampler2D uTexture;
uniform float kernelSize;

void main(void) {
    vec2 imageResolution = vec2(1920, 1080);
    vec2 uv = v_TexCoord;
    vec2 texelSize = vec2(1.) / imageResolution;
    vec4 color = texture(uTexture, v_TexCoord);

    //const float kernelSize = 15.0;
    vec3 boxBlurColor = vec3(0.0);
    float boxBlurDivisor = pow(2.0 * kernelSize + 1.0 , 2.0);
    // kernelSize = 1.0, then boxBlurDivisor = 9.0 // 3 by 3
    // kernelSize = 2.0, then boxBlurDivisor = 25.0 // 4 by 4
    // kernelSize = 3.0, then boxBlurDivisor = 49.0 // 6 by 6

    for (float i = -kernelSize; i <= kernelSize; i++) {
        for (float j = -kernelSize; j <= kernelSize; j++) {
            vec4 texture = texture(uTexture, uv + vec2(i, j) * texelSize);
            boxBlurColor = boxBlurColor + texture.rgb;
        }
    }
    boxBlurColor /= boxBlurDivisor;
    color = vec4(boxBlurColor, 1.0);

    fragColor = color;
}