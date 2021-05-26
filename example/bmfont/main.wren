import "gfx" for PixelData, Sprite, Layer
import "io" for File

class BMFontClass {

    type { _type }

    construct parse(line){
        var frags = line.split(" ").where{|x| x != ""}.toList
        _type = frags.removeAt(0)
        _data = {}
        for(el in frags){
            var pair = el.split("=")
            _data[pair[0]] = parseValue(pair[1])
        }
    }

    [key] { _data[key] }

    parseValue(val){
        if(val == ""){
            return null
        }else if(val.contains("\"")){
            return val[1...-1]
        } else if(val.contains(",")){
            return val.split(",").map{|x| Num.fromString(x)}.toList
        } else {
            return Num.fromString(val)
        }
    }
}

class BMFont {
    construct load(name){
        _pages = []
        _chars = {}

        for(line in File.load(name).replace("\r", "").split("\n").where{|x| x.count > 0}){
            var bmc = BMFontClass.parse(line)
            if(bmc.type == "info") {
            } else if(bmc.type == "common") {
                _lineHeight = bmc["lineHeight"]
            }  else if(bmc.type == "page") {
                _pages.add(PixelData.load(bmc["file"]))
            } else if(bmc.type == "chars") {
            } else if(bmc.type == "char") {
                _chars[String.fromCodePoint(bmc["id"])] = bmc
            }
        }
    }

    plotChar(pd, c, ox, oy){
        pd.image(_pages[c["page"]], ox,oy, c["x"],c["y"], c["width"], c["height"])
    }

    plotString(pd, str, ox, oy){
        var x = ox
        var y = oy
        for(cp in str){ 
            if(cp == "\n"){
                x = ox
                y = y+_lineHeight
                continue
            }
            var char = _chars[cp]
            if(char == null) continue
            plotChar(pd, char, x+char["xoffset"], y+char["yoffset"])
            x = x+char["xadvance"]
            Fiber.yield()
        }
    }
} 

var fnt = BMFont.load("altima.fnt")
var pd = PixelData.new(320,240)

var fib = Fiber.new {
    fnt.plotString(pd, File.load("license.txt"), 10,10)
}

Sprite.set(0, 0,0, pd.width, pd.height)

while(!fib.isDone){
    fib.call()
    pd.vram(0,0)
    Fiber.yield()
}
