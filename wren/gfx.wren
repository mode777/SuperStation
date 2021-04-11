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

foreign class PixelData {
  foreign static create(w,h)
  foreign static fromImage(path)
  foreign set(x,y,color)
}

// class Color {
//   static rgba(r,g,b,a){
//     return (a << 24) | (b << 16) | (g << 8) | r
//   }
// }