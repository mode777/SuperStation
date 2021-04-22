import "gfx" for PixelData, Sprite, Layer

var maketile = Fn.new {|x,y,c1,c2|
  var px = PixelData.new(2,2)
  px.set(0,0,c1)
  px.set(1,0,0xFF000000)
  px.set(0,1,c2)
  px.set(1,1,0xFFFFFFFF)
  px.vram(x,y)
}

var makelayer = Fn.new {|x,y,tx,ty|
  var px = PixelData.new(4,4)
  px.tile(1,1,0,0, true, false)
  px.tile(2,1,1,0, false,true)
  px.tile(1,2,1,0)
  px.tile(2,2,0,0)
  px.vram(x,y)
  Layer.set(0,x,y,4,4,tx,ty,2,2)
}

maketile.call(0,0,0xFF0000FF,0xFF00FF00)
maketile.call(2,0,0xFF00FFFF,0xFFFFFF00)
makelayer.call(0,2,0,0)

var x = 0
var y = 0

while(true){
  Layer.transform(0, x, y, 0, 10, 10)
  x = x+0.01
  y = y+0.01
  Fiber.yield()
}
