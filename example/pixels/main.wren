import "gfx" for PixelData, Sprite, VRAM

var pd = PixelData.new(3,3)

//0xAABBGGRR
pd.set(0,0,0xFFFF0000)
pd.set(2,0,0xFF00FFFF)
pd.set(1,1,0xFF0000FF)
pd.set(0,2,0xFF0000FF)
pd.set(2,2,0xFF0000FF)

VRAM.upload(pd)

Sprite.set(0,0,0,3,3,1,1,0)

Sprite.transform(0,100,100, 0, 32, 32)

while(true){
  Sprite.offset(0, 0, 0, 0.01)
  Fiber.yield()
}



