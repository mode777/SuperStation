import "gfx" for PixelData, Sprite, Layer
import "json" for JSON

var VISIBLE =  0x00800000

// load font metadata
var font = JSON.load("font2.json")
var line = 0
var lookup = {}
for(glyphStr in font["chars"]){
  var pos = 0
  var tilePos = 0
  for(glyph in glyphStr){
    var codes = []
    var width = font["widths"][line][pos]
    for(i in 0...width){
      codes.add(VISIBLE | (line << 8) | (tilePos + i))
    }
    tilePos = tilePos+width
    pos = pos+1

    lookup[glyph] = codes
  }
  line = line+1
}

// setup glyph and text data
var pixels = PixelData.load("font2.png")
var textData = PixelData.new(300, 20)

pixels.vram(0,0)
Layer.set(0, pixels.width, 0, textData.width, textData.height)
Layer.tileset(0, 0, 0, 1, 18)


var text = "ALEX HELLO WORLD XXX abcdefghijklmnopqrst"

var x = 0
var y = 0

for(g in text){
  if(x >= textData.width || g == "\n"){
    y = y+1
    x = 0
    continue
  }
  if(g == " "){
    x = x+2
    continue
  } 

  var codes = lookup[g] || lookup["?"]

  for(code in codes){
    textData.set(x,y,code)
    x = x+1
    if(x >= textData.width) break
  }
  textData.vram(pixels.width, 0)
  Fiber.yield()
}
