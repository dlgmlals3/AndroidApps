#version 310 es

precision highp float;

out vec4 FragColor;

void main(void) {
    vec2 uv = gl_PointCoord * 0. - 1.;
    vec2 circleCoord = 2.0 * gl_PointCoord - 1.0;
    FragColor = vec4(gl_PointCoord, 0.0, 1.0);
}