import "gfx" for PixelData, Sprite, Layer
import "json" for JSON

var VISIBLE =  0x00800000

// load font metadata
var font = JSON.load("font.json")
var line = 0
var lookup = {}
for(glyphStr in font["glyphs"]){
  var pos = 0
  var tilePos = 0
  for(glyph in glyphStr){
    var codes = []
    var width = font["widths"][line][pos]/2
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
var pixels = PixelData.load("font.png")
var textData = PixelData.new(150, 100)

pixels.vram(0,0)
Layer.set(0, pixels.width, 0, textData.width, textData.height, 0, 0, 2, 13)


var text = "Nach dem umstrittenen Urteil des Amtsgerichts Weimars, das unter anderem die Masken- und Testpflicht an zwei Schulen aufgehoben hatte, gibt es offenbar eine ähnliche Gerichtsentscheidung in Bayern. Das Amtsgericht im oberbayerischen Weilheim hat in einem Beschluss die Maskenpflicht an Schulen in dem Bundesland für verfassungswidrig und nichtig erklärt.

Eine Gerichtssprecherin bestätigte dem SPIEGEL auf Anfrage, dass eine solche Entscheidung am heutigen Dienstag ergangen sei – der Beschluss gelte jedoch nur für die konkreten Beteiligten. Ein im Netz kursierendes Papier, das wie eine anonymisierte Version der Gerichtsentscheidung aussieht und unter anderem vom rechtslastigen Blog »Tichys Einblick« verbreitet wird, sei hingegen nicht authentisch: Dabei, so formuliert es die Gerichtssprecherin, »handelt es sich nicht um den Originalbeschluss«.

Auch einen Ausschluss vom Unterricht untersagt das Gericht
Die Begründung ist der Gerichtssprecherin zufolge äußerst umfangreich. Sie könne daher mit Blick auf die online kursierende Version nicht sagen, »ob in der Begründung tatsächlich jedes Wort gleichlautend mit dem Originalbeschluss ist«. Der erste Anschein spreche indes dafür: »Der Tenor entspricht – wenn auch anonymisiert – der Originalfassung des Beschlusses.«

Demzufolge hat das Amtsgericht Weilheim der Schulleitung in diesem Fall untersagt, die betroffene Schülerin zum Tragen einer Schutzmaske auf dem Schulgelände zu zwingen. Zudem dürfe das Kind weder vom Klassenverband isoliert noch vom Unterricht ausgeschlossen werden.

Im thüringischen Weimar erregte zuletzt ein augenscheinlich ähnlicher Beschluss Aufsehen: Ein Einzelrichter am Amtsgericht hatte unter Verweis auf eine angebliche Kindeswohlgefährdung in einem Verfahren um zwei Jungen die Masken- und Testpflicht sowie das Abstandsgebot an zwei Schulen untersagt und Präsenzunterricht angeordnet. Verwaltungsrechtler bezweifeln, dass der Beschluss in dieser Form haltbar ist. Zudem prüft die Staatsanwaltschaft inzwischen mehrere Strafanzeigen gegen den zuständigen Richter, ihm wird Rechtsbeugung vorgeworfen.
angekündigt, gegen den Beschluss rechtlich vorzugehen, aus Bayern ist bislang keine Reaktion seitens der Landesregierung bekannt."

var x = 0
var y = 0

var fib = Fiber.new {
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
}



var fib2 = Fiber.new {

  var ox = -50
  var oy = 50

  while(true){
    Layer.transform(0,ox,oy)
    oy = oy-1
    Fiber.yield()
    Fiber.yield()
    Fiber.yield()
    Fiber.yield()
    Fiber.yield()
  }
}

while(true){
  fib2.call()
  fib.call()
  Fiber.yield()
}
