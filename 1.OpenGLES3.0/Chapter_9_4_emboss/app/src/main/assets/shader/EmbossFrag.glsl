#version 300 es
precision mediump float;

in vec2 TexCoord;
uniform vec2 pixelSize;
uniform sampler2D Tex1;
uniform float ScreenCoordinateX; // dlgmlals3

layout(location = 0) out vec4 outColor;

void main()
{
    //float ScreenCoordX = 522.0f;
    float EmbossBrightness = 0.7f;
    // Draw the red line for showing the difference b/w orignal and emboss effect.
    if(gl_FragCoord.x < ScreenCoordinateX + 3.0 && gl_FragCoord.x > ScreenCoordinateX - 3.0){
        outColor = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    }

    if (gl_FragCoord.x > ScreenCoordinateX){
        vec3 p00 = texture(Tex1, TexCoord).rgb;
        vec3 p01 = texture(Tex1, TexCoord + vec2(0.0, pixelSize.y)).rgb;
        vec3 diff = p00 - p01;
        float max = diff.r;
        if(abs(diff.g) > abs(max)){
            max = diff.g;
        }
        if(abs(diff.b) > abs(max)){
            max = diff.b;
        }
        float gray = clamp(max+EmbossBrightness, 0.0, 1.0);
        outColor = vec4(gray,gray,gray, 1.0);
    }
    else {
        outColor = texture(Tex1, TexCoord);
    }
    return;
}

