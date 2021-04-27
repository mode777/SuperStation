import "gfx" for PixelData, Sprite, Layer
import "json" for JSON
import "input" for Button

var spritesheet = JSON.load("spritesheet_complete.json")

PixelData.load(spritesheet["imagePath"]).vram(0,0)

var data = spritesheet["SubTexture"][0]
Sprite.set(0, data["x"], data["y"], data["width"], data["height"], data["width"] / 2, data["height"])
var ambulanceX = 80 
var ambulanceY = 60

data = spritesheet["SubTexture"][2]
Sprite.set(1, data["x"], data["y"], data["width"], data["height"], data["width"] / 2, data["height"])
var buggyX = 80
var buggyY = 180

var time = 0

while(true){
    if(Button.isDown(Button.RIGHT)){
        buggyX = buggyX+1
    }

    if(Button.isPressed(Button.RIGHT)){
        ambulanceX = ambulanceX+4
    }

    Sprite.transform(0,ambulanceX,ambulanceY+(time/8)%2)
    Sprite.transform(1,buggyX,buggyY+(time/5)%2)

    time = time+1
    Fiber.yield()
}