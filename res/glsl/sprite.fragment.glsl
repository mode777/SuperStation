uniform sampler2D uSampler;

varying mediump vec2 texcoord;
varying lowp vec4 vColor;
varying lowp float vIntensity;

void main(void) {
  mediump vec2 uv = mod(texcoord, vec2(1.0)); 
  lowp vec4 color = texture2D(uSampler, texcoord);
  color.a *= 1.0-vColor.a;
  color.rgb = mix(color.rgb, vColor.rgb, vIntensity);
  gl_FragColor = color;
}