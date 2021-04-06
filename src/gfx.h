#ifndef GFX_H
#define GFX_H

#include <GLES2/gl2.h>

#include "gfx_common.h"
#include "error.h"
#include "vram.h"
#include "sprites.h"
#include "framebuffer.h"
#include "layer.h"

typedef struct {
  sst_Sprites sprites;
  sst_VRAM vram;
  sst_Layers layers;
  // sst_Quad layers[SST_MAX_LAYERS];
  // sst_Program layerProgram;
  // sst_Geometry spriteGeometry;
  // sst_Geometry layerGeometry;
  sst_Framebuffer framebuffer; 
} sst_Gfx;

sst_ErrorCode sst_gfx_init(sst_Gfx* state);
sst_ErrorCode sst_gfx_update(sst_Gfx* state);
sst_ErrorCode sst_gfx_draw(sst_Gfx* state);

#endif