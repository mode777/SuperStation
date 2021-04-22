uniform sampler2D texture;

varying mediump vec2 texcoord;
varying mediump vec4 vColor;
varying mediump float vIntensity;

uniform mediump vec2 texSize;
uniform mediump float prio;
uniform mediump vec2 tilesize;
uniform mediump float time;
uniform mediump vec2 mapSize;
uniform mediump vec2 mapOffset;
uniform mediump vec2 tileOffset;
uniform mediump int clampMode;

void main(void) {
  mediump vec2 inp = texcoord;

  // water wobble
  //inp.x += mod(gl_FragCoord.y, 2.0) * -sin(gl_FragCoord.y / 50.0 + time) + mod(gl_FragCoord.y+1.0, 2.0) * sin(gl_FragCoord.y / 35.0 + time);
  //inp.y += cos(gl_FragCoord.y / 30.0 + time);
  //inp.x += sin(gl_FragCoord.y / 50.0 + time);
  
  inp = clampMode == 0 
    // repeat bg
    ? mapOffset + mod(inp, mapSize) 
    // clamp bg
    : mapOffset + clamp(floor(inp), vec2(0,0), (mapSize-vec2(1.0,1.0))) + fract(inp);

  mediump vec4 tile = texture2D(texture, (floor(inp)+0.5) / texSize);
  tile *= 255.0;
  mediump int flags1 = int(tile.z);
  mediump int flags2 = int(tile.w);
  mediump float drawFlag = mod(float(flags1/128), 2.0);
  mediump float flipXFlag = mod(float(flags1/64), 2.0);
  mediump float flipYFlag = mod(float(flags1/32), 2.0);
   
  mediump vec2 oneTile = (texSize / tilesize);

  mediump vec2 local = vec2(1.0-flipXFlag, 1.0-flipYFlag) * fract(inp) + vec2(flipXFlag, flipYFlag) * (1.0 - fract(inp)); 
  
  mediump vec2 offset = ((floor(local * tilesize + tileOffset) + 0.5) / tilesize);
  mediump vec2 uv = (tile.xy + offset) / oneTile;

  mediump vec4 color = texture2D(texture, uv);
  color.a *= (1.0-vColor.a) * drawFlag;
  color.rgb = mix(color.rgb, vColor.rgb, vIntensity);
  gl_FragColor = color;
}