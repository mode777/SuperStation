attribute vec4 coordUv;
attribute vec2 scale;
attribute vec2 trans;
attribute float rot;
attribute vec2 prioIntensity;
attribute vec4 color;

uniform mediump vec2 texSize;
uniform vec2 size;
uniform float prio;

varying vec2 texcoord;
varying vec4 vColor;
varying float vIntensity;

void main(void) {
  vColor = color;
  vIntensity = prioIntensity.y / 255.0;

  vec2 uv = coordUv.zw / texSize;
  texcoord = uv; 

  float r = (rot / 5215.0);
  float s = sin(r);
  float c = cos(r);
  float sx = scale.x;
  float sy = scale.y;
  float sprio = prioIntensity.x;
  float mult = step(prio, sprio) * step(sprio, prio);
  sx *= mult;
  sy *= mult;

  float m0 = sx * c;
  float m1 = sx * s;

  float m3 = sy * -s;
  float m4 = sy * c;
  
  float m6 = trans.x;
  float m7 = trans.y;

  mat3 transformation = mat3(m0, m1, 0.0, m3, m4, 0.0, m6, m7, 1.0);
  //transformation = mat3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, m6, m7, 1.0);

  vec3 transformed = transformation * vec3(coordUv.xy, 1.0);

  vec2 xy = (transformed.xy / (size / 2.0) - 1.0) * vec2(1.0, -1.0);
  gl_Position = vec4(xy, 0.0, 1.0);
}