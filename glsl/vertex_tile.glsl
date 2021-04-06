attribute vec4 coordUv;
attribute vec2 scale;
attribute vec2 trans;
attribute float rot;
attribute vec2 prioIntensity;
attribute vec4 color;

uniform float prio;
uniform vec2 size;
uniform mediump vec2 texSize;
uniform mediump vec2 tilesize;

varying vec2 texcoord;
varying vec4 vColor;
varying float vIntensity;

void main(void) {
    vColor = color;
    vIntensity = prioIntensity.y / 255.0;

    vec2 screensize = size / tilesize;
    vec2 uv = (coordUv.zw * screensize) - (screensize / 2.0);// - (size / (tilesize * pixelscale * 2.0));

    float r = -(rot / 5215.0);
    float s = sin(r);
    float c = cos(r);
    float sx = 1.0 / scale.x;
    float sy = 1.0 / scale.y;
    float sprio = prioIntensity.x;

    float m0 = sx * c;
    float m1 = sx * s;

    float m3 = sy * -s;
    float m4 = sy * c;

    vec2 translation = -(trans / tilesize) + (screensize / 2.0);

    mat3 transformation = mat3(m0, m1, 0.0, m3, m4, 0.0, translation.x, translation.y, 1.0);
    texcoord = (transformation * vec3(uv, 1.0)).xy;

    float mult = step(prio, sprio) * step(sprio, prio);
    vec2 xy = mult * coordUv.xy * vec2(1.0, -1.0);
    gl_Position = vec4(xy, 0.0, 1.0);
}