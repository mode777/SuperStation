attribute vec4 coordUv;

uniform vec2 scale;

varying vec2 texcoord;

void main(void) {
  texcoord = coordUv.zw; 
  gl_Position = vec4(coordUv.xy * scale, 0.0, 1.0);
}