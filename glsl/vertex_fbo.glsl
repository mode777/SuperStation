attribute vec4 coordUv;

varying vec2 texcoord;

void main(void) {
  texcoord = coordUv.zw; 
  gl_Position = vec4(coordUv.xy, 0.0, 1.0);
}