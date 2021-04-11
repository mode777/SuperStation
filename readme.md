# SuperStation 

SuperStation is a 16-bit fantasy console. Think of it as a Super Nintendo on steroids. In terms of limitations it falls somewhere between PICO-8 and Löve2D. You use the [Wren Programming Language](https://wren.io) to write games.  

## Features and Restrictions

* 1024 simultaneous sprites that can be moved, scaled and rotated individually
* 8 simultaneous tile layers that can use affine transformations (Mode 7)
* 1024px * 1024px of video memory for graphics (RGBA)
* Fast OpenGL ES 2.0 based renderer that will maintain 60 fps even on devices as low as the Rasperry Pi 1

## Getting started

Create a new folder called `myproject`

`myproject/main.wren`
```wren
import "gfx" for Sprite, VRAM, PixelData

// Load an image and upload it to video ram upper-left corner
var pd = PixelData.fromImage("image.png")
VRAM.upload(pd, 0, 0)

// Set Sprite 1 to VRAM location 0,0 size: 16x16
Sprite.set(1,0,0,16,16)

var rotate = 0

// Run game loop
while(true){
  // Move sprite to coordinates 100, 100 and rotate by r
  Sprite.transform(1, 100, 100, r)
  r = r + 0.01
  // Return control for the console to render
  Fiber.yield()
}
```

Start SuperStation and drag the folder on the window.

## Building

SuperStation uses CMake for building.

### Dependencies

* SDL2

On Desktop platforms SuperStation relies on **ANGLE** for OpenGL ES 2.0 and EGL implementation. As the build process is rather lengthy, binaries are included.

SuperStation also relies on these libraries that are created during build:
* Wren
* jsmn
* stb_image
* stb_ds

### Windows

Building on Windows has only been tested with Mingw32

```
mkdir build
cd build
cmake ..
cmake --build .
```

### WebAssembly

Install emscripten according to instructions

```
mkdir build-wasm
cd build-wasm
source emsdk_env.sh
emconfigure cmake ..
emmake make
```

WebAssembly builds call some methods in `superstation.html` to load files asynchronously. Make sure to serve superstation.html from a fileserver.