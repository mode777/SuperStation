import "gfx" for VRAM, PixelData, Sprite, Layer

var pd = PixelData.load("sheet.png")
VRAM.upload(pd, 0, 256)

var VISIBLE =  0x00800000
var BOX = VISIBLE | 0x0408
var LADDER = VISIBLE | 0x0407
var CHEST = VISIBLE | 0x040E
var SIGN = VISIBLE | 0x010E
var FLOWER = VISIBLE | 0x050E
var FLOWER2 = VISIBLE | 0x060E
var FENCE = VISIBLE | 0x020E
var FENCE2 = VISIBLE | 0x030E

var tiles = [BOX, LADDER, CHEST, SIGN, FLOWER, FLOWER2, FENCE, FENCE2]

for(layer in 0...8){
  pd = PixelData.new(3,3)
  pd.set(1,1,tiles[layer])
  VRAM.upload(pd, 256+(layer*3), 0)
  Layer.set(layer, 256+(layer*3), 0, 3, 3, 0, 256, 16, 16)
}


//Sprite.set(0,0,256,256,256,0,0,6)

//var x = -256*16
var x = 0
var y = 0

while(true){
  Layer.transform(0,160,120,x/100,1,1)
  Layer.transform(1,0,0,x/75)
  Layer.transform(2,320,0,x/50)
  Layer.transform(3,0,240,x/25)
  Layer.transform(4,320,240,-x/100)
  Layer.transform(5,160,120,-x/75)
  Layer.transform(6,0,0,-x/50)
  Layer.transform(7,320,0,-x/25)
  x = x + 0.1
  y = y + 0.1
  Fiber.yield()
}