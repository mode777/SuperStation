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
    free((void*)pixels);
    wrenError(vm, "Image too big for VRAM");
    return;
  }

  sst_ErrorCode error = sst_vram_upload_image(&state->gfx.vram, x, y, (unsigned char*)pixels, w, h);
  free((void*)pixels);
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

  const char* imgRoot = state->root;
  const char* path = sst_string_join(imgRoot, img);
  sst_ErrorCode error = sst_io_readfile(path, &png, &png_size);
  free((void*)path);
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
}

static void pixeldata_set_3(WrenVM* vm){
  unsigned int* data = wrenGetSlotForeign(vm, 0);
  unsigned int w = data[0];
  unsigned int h = data[1];
  unsigned int* pixels = &data[2];
  unsigned int x = wrenGetSlotDouble(vm, 1);
  unsigned int y = wrenGetSlotDouble(vm, 2);
  unsigned int pixel = wrenGetSlotDouble(vm, 3);
  if(x >= w || y >= h){
    wrenError(vm, "Pixel out of range");
  }
  pixels[y*w+x] = pixel;
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

static void layer_set_9(WrenVM* vm){
  int id = wrenGetSlotDouble(vm, 1);
  int ox = wrenGetSlotDouble(vm, 2);
  int oy = wrenGetSlotDouble(vm, 3);
  int w = wrenGetSlotDouble(vm, 4);
  int h = wrenGetSlotDouble(vm, 5);
  int tox = wrenGetSlotDouble(vm, 6);
  int toy = wrenGetSlotDouble(vm, 7);
  int tw = wrenGetSlotDouble(vm, 8);
  int th = wrenGetSlotDouble(vm, 9);

  sst_LayerAttribs* q = &sst_wren_get_state(vm)->gfx.layers.attributes[id];

  q->offsetX = ox;
  q->offsetY = oy;
  q->width = w;
  q->height = h;
  q->tileOffsetX = tox;
  q->tileOffsetY = toy;
  q->tileW = tw;
  q->tileH = th;
  q->enabled = true;
}

sst_ErrorCode sst_gfx_wren_register(WrenVM* vm){
  sst_wren_register_method(vm, "gfx.Sprite.transform(_,_,_,_,_,_)", sprite_transform_6);
  sst_wren_register_method(vm, "gfx.Sprite.offset(_,_,_,_,_,_)", sprite_offset_6);
  sst_wren_register_method(vm, "gfx.Sprite.set(_,_,_,_,_,_,_,_)", sprite_set_8);
  sst_wren_register_method(vm, "gfx.Sprite.unset(_)", sprite_unset_1);
  sst_wren_register_method(vm, "gfx.VRAM.upload(_,_,_)", vram_upload_3);
  sst_wren_register_method(vm, "gfx.PixelData.create(_,_)", pixeldata_create_2);
  sst_wren_register_method(vm, "gfx.PixelData.fromImage(_)", pixeldata_fromImage_1);
  sst_wren_register_method(vm, "gfx.PixelData.set(_,_,_)", pixeldata_set_3);
  sst_wren_register_method(vm, "gfx.Layer.transform(_,_,_,_,_,_)", layer_transform_6);
  sst_wren_register_method(vm, "gfx.Layer.set(_,_,_,_,_,_,_,_,_)", layer_set_9);
  return sst_NoError;
}