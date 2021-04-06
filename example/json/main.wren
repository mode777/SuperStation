import "json" for JSON

var json = JSON.load("example.json")

System.print(json)

while(true){
  Fiber.yield()
}

