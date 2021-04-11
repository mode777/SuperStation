#ifndef SPRITES_H
#define SPRITES_H

#include <GLES2/gl2.h>

#include "error.h"
#include "gfx_common.h"
#include "quad.h"

#define SST_MAX_SPRITES 1024

typedef struct {
  sst_Quad quads[SST_MAX_SPRITES];
  sst_Program program;
  GLuint buffer;
  GLuint indexBuffer;
  int spritesOnLayer[SST_MAX_LAYERS];
} sst_Sprites;

sst_ErrorCode sst_sprites_init(sst_Sprites* sprites);
sst_ErrorCode sst_sprites_update(sst_Sprites* sprites);
sst_ErrorCode sst_sprites_draw(sst_Sprites* sprites);
sst_ErrorCode sst_sprites_reset(sst_Sprites* sprites);

#endif