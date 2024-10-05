#version 310 es

//#define NUM_PARTICLE 95.
//#define NUM_EXPLOSION 5.
#define PI 3.141592

precision highp float;

out vec4 FragColor;
uniform float u_time;
uniform float num_particle;
uniform float num_explosion;

vec2 Hash12(float t) { // 1
   float x = fract(sin(t * 674.4) * 453.2);
   float y = fract(sin((t + x) * 714.2) * 263.3);
   return vec2(x, y);
}

vec2 HashPolar12(float t) {
    float theta = fract(sin(t * 674.4) * 453.2) * PI * PI;
    float r = fract(sin((t + theta) * 714.2) * 263.3) * .3;
    return vec2(sin(theta), cos(theta)) * r;
}

float Explosion(vec2 uv, float t) {
    float explod = 0.;
    for (float i=0.; i< num_particle; i++) {
        vec2 dir = HashPolar12((i + .1)); // 6

        float brightness = mix(.0001, .0007, smoothstep(.1, .7, t)); // 7
        brightness *= smoothstep(1., .7, t); // 12
        float sparkle = sin(t * i * .5) * .5 + .5; // 8

        float d = length(uv - dir * t);
        explod += brightness * sparkle / d;
    }
    return explod;
}

void main(void) {
    vec2 screenSize = vec2(1080, 2400);
    vec2 uv = gl_FragCoord.xy / screenSize - .5;
    vec3 col = vec3(0.);

    float iTime = u_time;
    float firewlaks = 0.;

    for (float i=0.; i < num_explosion; i++) {
       float t = iTime + (i / num_explosion); // 15
       float ft = floor(t); // 11

       vec3 color = sin(4. * vec3(.34, .54, .43) * ft) * .25 + .75; // 10
       vec2 offs = Hash12(i + 1. + ft) - .5; // 14
       offs *= vec2(1.2, 1.5);
       col += Explosion(uv - offs, fract(t)) * color;
    }

    //col = vec3(uv, 0.);
    FragColor = vec4(col * .3, 0.);
}