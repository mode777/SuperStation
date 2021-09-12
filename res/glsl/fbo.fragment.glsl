uniform sampler2D texture;
varying mediump vec2 texcoord;

void main(void) {
  gl_FragColor = texture2D(texture, texcoord);
}