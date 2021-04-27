import "gfx" for PixelData, Sprite, Layer
import "random" for Random

var Colors = [
    0x0007071F,
    0xFF070F47,
    0xFF071F67,
    0xFF07278F,
    0xFF073FAF,
    0xFF0747C7,
    0xFF0757DE,
    0xFF075FD6,
    0xFF0F67D7,
    0xFF0F77CF,
    0xFF1787CF,
    0xFF178FC7,
    0xFF1F9FBE,
    0xFF27A7BE,
    0xFF2FAFBF,
    0xFF2FB7B7,
    0xFF6FCFCF,
    0xFFC7EFEF,
    0xFFFFFFFF
]

// As we don't deal with indiced colors we need to calculate
// a lookup for each colors position within the array
var Lookup = {}
for(i in 0...Colors.count){
    Lookup[Colors[i]] = i
}

// https://fabiensanglard.net/doom_fire_psx/
class DoomFire {

    construct new(w, h){
        _w = w
        _h = h
        _rand = Random.new()
        
        //Sprite.set(0,0,0,_w,_h)
        //Sprite.transform(0,0,0,0,320/_w,240/_h)

        var ld = PixelData.new(1,1)
        ld.tile(0,0,0,0)
        ld.vram(_w,0)
        Layer.set(0, _w, 0, ld.width, ld.height, 0, 0, _w, _h)
        Layer.transform(0,0,0,0,1,240/_h)

        _pd = PixelData.new(_w, _h)
        _pd.rect(0,0,_w,_h,Colors[0])

        for(x in 0..._w){
            _pd.set(x,_h-1,Colors[Colors.count-1])
        }

        _pd.vram(0,0)
    }

    spreadFire(x,y) {
        var pixel = _pd.get(x,y)

        if(pixel == Colors[0]) {
            _pd.set(x,y-1,Colors[0])
        } else {
            var randIdx = _rand.int(3)
            var dst = x - randIdx + 1
            var newPixel = Colors[Lookup[pixel] - (randIdx & 1)]
            _pd.set(dst, y-1, newPixel)
        }
    }
    
    doFire() {
        var half = (_w/2).floor 
        for(z in 0...4){
            for(x in 0..._w/4){
                for(y in 1..._h){
                    spreadFire((x*4)+z,y)
                }
            }
            _pd.vram(0,0)
            Fiber.yield()
        }
    }
}

var fire = DoomFire.new(160,160)

var x = 0

while(true){
    fire.doFire()
    Layer.transform(0,x,0)
    x = x - 1
}

