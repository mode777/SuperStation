class Sprite {
  
  static transform(i, x, y) { Sprite.transform(i, x, y, 0, 1, 1) }
  static transform(i, x, y, r) { Sprite.transform(i, x, y, r, 1, 1) }
  foreign static transform(i, x, y, r, sx, sy)

  static offset(i, x, y) { Sprite.offset(i, x, y, 0, 0, 0) }
  static offset(i, x, y, r) { Sprite.offset(i, x, y, r, 0, 0) }
  foreign static offset(i, x, y, r, sx, sy)

  static set(i, x, y, w, h) { Sprite.set(i, x, y, w, h, 0, 0, 0) }
  static set(i, x, y, w, h, ox, oy) { Sprite.set(i, x, y, w, h, ox, oy, 0) }
  foreign static set(i, x, y, w, h, ox, oy, layer)
}

class Layer {
  static transform(i, x, y) { Layer.transform(i, x, y, 0, 1, 1) }
  static transform(i, x, y, r) { Layer.transform(i, x, y, r, 1, 1) }
  foreign static transform(i, x, y, r, sx, sy)
  foreign static set(i, x, y, w, h, tilesX, tilesY, tileW, tileH)
  static set(i, x, y, w, h) { Layer.set(i, x, y, w, h, 0, 0, 16, 16) }
}

class VRAM {
  static upload(path) { VRAM.upload(path, 0, 0) }
  foreign static upload(path, x, y)
}

var VISIBLE =  0x00800000
var FLIPX =  0x00400000
var FLIPY =  0x00200000

foreign class PixelData {
  foreign static new(w,h)
  foreign static load(path)
  foreign width
  foreign height
  vram(x,y) { VRAM.upload(this,x,y) }
  foreign set(x,y,color)
  foreign get(x,y)
  foreign rect(x,y,w,h,color)
  tile(x,y,tileX,tileY, flipX, flipY) { set(x,y,VISIBLE | (tileY << 8) | tileX | (flipX ? FLIPX : 0) | (flipY ? FLIPY : 0)) }
  tile(x,y,tileX,tileY) { set(x,y,VISIBLE | (tileY << 8) | tileX) }
}

// class Color {
//   static rgba(r,g,b,a){
//     return (a << 24) | (b << 16) | (g << 8) | r
//   }
// }