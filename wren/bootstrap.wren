var update__ = Fn.new { |fiber|
  if(!fiber.isDone){
    fiber.call()
    return true
  }
  return false
}

var main__ = Fiber.new { 
  import "main" 
}

var repl__ = Fiber.new {|str|
  import "meta" for Meta

  Meta.compile("import \"gfx\" for Sprite, VRAM").call()
   
  while(true){
    var result = Fiber.new {
      var fiber = Meta.compile(str)
      return fiber.call()
    }.try()
    str = Fiber.yield(result)
  }
}

