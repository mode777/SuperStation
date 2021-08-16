#include "gfx_wren.h"

#include "sst_common.h"
#include "sst_wren.h"
#include "quad.h"
#include "io.h"
#include "vram.h"
#include "error.h"
#include "image.h"

static void sprite_transform_6(WrenVM* vm){
  int id = wrenGetSlotDouble(vm, 1);
  int x = wrenGetSlotDouble(vm, 2);
  int y = wrenGetSlotDouble(vm, 3);
  double r = wrenGetSlotDouble(vm, 4);
  double sx = wrenGetSlotDouble(vm, 5);
  double sy = wrenGetSlotDouble(vm, 6);

  sst_Quad* q = &sst_wren_get_state(vm)->gfx.sprites.quads[id];

  sst_quad_set_translation(q, x, y);
  sst_quad_set_rotation(q, r);
  sst_quad_set_scale(q, sx, sy);
}

static void sprite_offset_6(WrenVM* vm){
  int id = wrenGetSlotDouble(vm, 1);
  int x = wrenGetSlotDouble(vm, 2);
  int y = wrenGetSlotDouble(vm, 3);
  double r = wrenGetSlotDouble(vm, 4);
  double sx = wrenGetSlotDouble(vm, 5);
  double sy = wrenGetSlotDouble(vm, 6);

  sst_Quad* q = &sst_wren_get_state(vm)->gfx.sprites.quads[id];

  sst_quad_offset_translation(q, x, y);
  sst_quad_offset_rotation(q, r);
  sst_quad_offset_scale(q, sx, sy);
}

static void sprite_set_8(WrenVM* vm){
  int id = wrenGetSlotDouble(vm, 1);
  int x = wrenGetSlotDouble(vm, 2);
  int y = wrenGetSlotDouble(vm, 3);
  int w = wrenGetSlotDouble(vm, 4);
  int h = wrenGetSlotDouble(vm, 5);
  int ox = wrenGetSlotDouble(vm, 6);
  int oy = wrenGetSlotDouble(vm, 7);
  int layer = wrenGetSlotDouble(vm, 8);

  sst_State* state = sst_wren_get_state(vm);
  sst_Quad* q = &state->gfx.sprites.quads[id];

  sst_quad_set_shape(q, 0, 0, w, h, ox, oy);
  sst_quad_set_source(q, x, y, w, h);

  int oldLayer = q->corners[0].prio;
  if(oldLayer == layer+1) return;
  if(oldLayer != 0) state->gfx.sprites.spritesOnLayer[oldLayer-1]--;
  state->gfx.sprites.spritesOnLayer[layer]++;
  sst_quad_set_layer(q, layer+1);
}

static void sprite_unset_1(WrenVM* vm){
  int id = wrenGetSlotDouble(vm, 1);

  sst_State* state = sst_wren_get_state(vm);
  sst_Quad* q = &state->gfx.sprites.quads[id];

  int oldLayer = q->corners[0].prio;
  if(oldLayer != 0) state->gfx.sprites.spritesOnLayer[oldLayer-1]--;

  sst_quad_reset(q);
}

static void vram_upload_3(WrenVM* vm){
  sst_State* state = sst_wren_get_state(vm);
  unsigned int* data = wrenGetSlotForeign(vm, 1);
  unsigned int w = data[0];
  unsigned int h = data[1];
  unsigned int* pixels = &data[2];
  
  int x = wrenGetSlotDouble(vm, 2);
  int y = wrenGetSlotDouble(vm, 3);

  if((x + w) > SST_VRAM_WIDTH || (y + h) > SST_VRAM_HEIGHT){
    wrenError(vm, "Image too big for VRAM");
    return;
  }

  sst_VRAM_Location loc = { .x = x, .y = y };
  sst_ErrorCode error = sst_vram_upload_image(&state->gfx.vram, loc, (unsigned char*)pixels, w, h);
  
  if(error != sst_NoError){
    wrenError(vm, sst_error_get());
    sst_error_clear();
    return;
  }
}

static void pixeldata_create_2(WrenVM* vm){
  unsigned int w = wrenGetSlotDouble(vm, 1);
  unsigned int h = wrenGetSlotDouble(vm, 2);
  unsigned int* pixelData = wrenSetSlotNewForeign(vm, 0, 0, w*h*sizeof(int) + 2*sizeof(int));
  pixelData[0] = w;
  pixelData[1] = h;
}

static void pixeldata_fromImage_1(WrenVM* vm){
  sst_State* state = sst_wren_get_state(vm);
  const char* img = wrenGetSlotString(vm, 1);
  
  const char* png;
  const char* pixels;
  size_t png_size, w, h;

  sst_ErrorCode error = sst_wren_load_resource(vm, img, (unsigned char**)&png, &png_size);
  
  if(error != sst_NoError){
    wrenError(vm, sst_error_get());
    sst_error_clear();
    return;
  }
  error = sst_image_decode((unsigned char*)png, png_size, (unsigned char**)&pixels, &w, &h);
  free((void*)png);

  if(error != sst_NoError){
    wrenError(vm, sst_error_get());
    sst_error_clear();
    return;
  }

  unsigned int* pixelData = wrenSetSlotNewForeign(vm, 0, 0, w*h*sizeof(int) + 2*sizeof(int));
  pixelData[0] = w;
  pixelData[1] = h;
  // copy to managed memory - TODO: Find a more memory economic solution
  memcpy(&pixelData[2], pixels, w*h*sizeof(int));
  free((void*)pixels);
}

static void pixeldata_set_3(WrenVM* vm){
  unsigned int* data = wrenGetSlotForeign(vm, 0);
  unsigned int w = data[0];
  unsigned int h = data[1];
  unsigned int* pixels = &data[2];
  unsigned int x = wrenGetSlotDouble(vm, 1);
  unsigned int y = wrenGetSlotDouble(vm, 2);
  unsigned int pixel = wrenGetSlotDouble(vm, 3);
  pixels[(y%h)*w+(x%w)] = pixel;
}

static void pixeldata_rect_5(WrenVM* vm){
  unsigned int* data = wrenGetSlotForeign(vm, 0);
  unsigned int w = data[0];
  unsigned int h = data[1];
  unsigned int* pixels = &data[2];
  unsigned int x = wrenGetSlotDouble(vm, 1);
  unsigned int y = wrenGetSlotDouble(vm, 2);
  unsigned int rw = wrenGetSlotDouble(vm, 3);
  unsigned int rh = wrenGetSlotDouble(vm, 4);
  unsigned int color = wrenGetSlotDouble(vm, 5);
  if(x+rw > w || y+rh > h){
    wrenError(vm, "Pixel out of range");
    return;
  }
  for (size_t cy = y; cy < y+rh; cy++)
  {
    for (size_t cx = x; cx < x+rw; cx++)
    {
      pixels[cy*w+cx] = color;
    }
  }  
}

static void pixeldata_image_7(WrenVM* vm){
  unsigned int* data = wrenGetSlotForeign(vm, 0);
  unsigned int tw = data[0];
  unsigned int th = data[1];
  sst_ColorAlpha* tpixels = (sst_ColorAlpha*)&data[2];
  data = wrenGetSlotForeign(vm, 1);
  unsigned int sw = data[0];
  unsigned int sh = data[1];
  sst_ColorAlpha* spixels = (sst_ColorAlpha*)&data[2];

  int tx = wrenGetSlotDouble(vm, 2);
  int ty = wrenGetSlotDouble(vm, 3);
  int sx = wrenGetSlotDouble(vm, 4);
  int sy = wrenGetSlotDouble(vm, 5);
  unsigned int w = wrenGetSlotDouble(vm, 6);
  unsigned int h = wrenGetSlotDouble(vm, 7);

  if(tx < 0 || ty < 0 || sx < 0 || sy < 0 || tx+w > tw || ty+h > th || sx+w > sw || sy+h > sh){
    wrenError(vm, "Pixel out of range");
    return;
  }
  for (size_t cy = 0; cy < h; cy++)
  {
    for (size_t cx = 0; cx < w; cx++)
    {
      sst_ColorAlpha source = spixels[(sy+cy)*sw+cx+sx];
      sst_ColorAlpha target = tpixels[(ty+cy)*tw+cx+tx];
      sst_ColorAlpha blended = { 
        .r = source.r + target.r*(1-source.a),
        .g = source.g + target.g*(1-source.a),
        .b = source.b + target.b*(1-source.a),
        .a = source.a + target.a*(1-source.a),
      };

      tpixels[(ty+cy)*tw+cx+tx] = blended;
    }
  }  
}

static void pixeldata_get_2(WrenVM* vm){
  unsigned int* data = wrenGetSlotForeign(vm, 0);
  unsigned int w = data[0];
  unsigned int h = data[1];
  unsigned int* pixels = &data[2];
  unsigned int x = wrenGetSlotDouble(vm, 1);
  unsigned int y = wrenGetSlotDouble(vm, 2);
  wrenSetSlotDouble(vm, 0, pixels[(y%h)*w+(x%w)]);
}

static void pixeldata_width(WrenVM* vm){
  unsigned int* data = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, data[0]);
}

static void pixeldata_height(WrenVM* vm){
  unsigned int* data = wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, data[1]);
}

static void layer_transform_6(WrenVM* vm){
  int id = wrenGetSlotDouble(vm, 1);
  int x = wrenGetSlotDouble(vm, 2);
  int y = wrenGetSlotDouble(vm, 3);
  double r = wrenGetSlotDouble(vm, 4);
  double sx = wrenGetSlotDouble(vm, 5);
  double sy = wrenGetSlotDouble(vm, 6);

  sst_Quad* q = &sst_wren_get_state(vm)->gfx.layers.quads[id];

  sst_quad_set_translation(q, x, y);
  sst_quad_set_rotation(q, r);
  sst_quad_set_scale(q, sx, sy);
}

static void layer_tileset_5(WrenVM* vm){
  int id = wrenGetSlotDouble(vm, 1);
  int tox = wrenGetSlotDouble(vm, 2);
  int toy = wrenGetSlotDouble(vm, 3);
  int tw = wrenGetSlotDouble(vm, 4);
  int th = wrenGetSlotDouble(vm, 5);

  sst_LayerAttribs* q = &sst_wren_get_state(vm)->gfx.layers.attributes[id];

  q->tileOffsetX = tox;
  q->tileOffsetY = toy;
  q->tileW = tw;
  q->tileH = th;
}

static void layer_set_5(WrenVM* vm){
  int id = wrenGetSlotDouble(vm, 1);
  int ox = wrenGetSlotDouble(vm, 2);
  int oy = wrenGetSlotDouble(vm, 3);
  int w = wrenGetSlotDouble(vm, 4);
  int h = wrenGetSlotDouble(vm, 5);

  sst_LayerAttribs* q = &sst_wren_get_state(vm)->gfx.layers.attributes[id];

  q->offsetX = ox;
  q->offsetY = oy;
  q->width = w;
  q->height = h;
  q->enabled = true;
}

sst_ErrorCode sst_gfx_wren_register(WrenVM* vm){
  sst_wren_register_method(vm, "gfx.Sprite.transform(_,_,_,_,_,_)", sprite_transform_6);
  sst_wren_register_method(vm, "gfx.Sprite.offset(_,_,_,_,_,_)", sprite_offset_6);
  sst_wren_register_method(vm, "gfx.Sprite.set(_,_,_,_,_,_,_,_)", sprite_set_8);
  sst_wren_register_method(vm, "gfx.Sprite.unset(_)", sprite_unset_1);
  sst_wren_register_method(vm, "gfx.VRAM.upload(_,_,_)", vram_upload_3);
  sst_wren_register_method(vm, "gfx.PixelData.new(_,_)", pixeldata_create_2);
  sst_wren_register_method(vm, "gfx.PixelData.load(_)", pixeldata_fromImage_1);
  sst_wren_register_method(vm, "gfx.PixelData.set(_,_,_)", pixeldata_set_3);
  sst_wren_register_method(vm, "gfx.PixelData.rect(_,_,_,_,_)", pixeldata_rect_5);
  sst_wren_register_method(vm, "gfx.PixelData.image(_,_,_,_,_,_,_)", pixeldata_image_7);
  sst_wren_register_method(vm, "gfx.PixelData.get(_,_)", pixeldata_get_2);
  sst_wren_register_method(vm, "gfx.PixelData.width", pixeldata_width);
  sst_wren_register_method(vm, "gfx.PixelData.height", pixeldata_height);
  sst_wren_register_method(vm, "gfx.Layer.transform(_,_,_,_,_,_)", layer_transform_6);
  sst_wren_register_method(vm, "gfx.Layer.set(_,_,_,_,_)", layer_set_5);
  sst_wren_register_method(vm, "gfx.Layer.tileset(_,_,_,_,_)", layer_tileset_5);
  return sst_NoError;
}