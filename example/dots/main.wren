import "gfx" for Sprite, VRAM, PixelData
import "random" for Random

var pd = PixelData.fromImage("pal.png")

VRAM.upload(pd)

var r = Random.new()

var x = 0
var y = 0

var fibers = []

for(i in 0...1000){
  fibers.add(Fiber.new {
    while(true){
      var x = r.int(480) 
      var y = -r.int(100)
      var vx = r.float(4) - 2
      var vy = 0
      var lifetime = 800 + r.int(280)

      Sprite.set(i,r.int(256),r.int(256),8,8, 4, 4)
      
      while(lifetime > 0){
        Sprite.transform(i, x, y)
        x = x + vx
        y = y + vy
        //lifetime = lifetime - 1

        vx = vx * 0.9999

        if(x > 480 || x < 0) vx = -vx 
        if(y > 270 || y < 0) vy = -vy * 0.6

        if(y<270) vy = vy + 0.2

        Fiber.yield() 
      }
    }  
  })
}

while(true){
  for(f in fibers){
    if(!f.isDone) f.call()
  }
  Fiber.yield()
}