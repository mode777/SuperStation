#ifndef VRAM_H
#define VRAM_H

#include "error.h"
#include "gfx_common.h"

#define SST_VRAM_WIDTH 1024
#define SST_VRAM_HEIGHT 1024

typedef struct {
  unsigned short x;
  unsigned short y;
} sst_VRAM_Location;

typedef struct {
  GLuint texture;
} sst_VRAM;

sst_ErrorCode sst_vram_init(sst_VRAM* vram);
sst_ErrorCode sst_vram_upload_image(sst_VRAM* vram, sst_VRAM_Location loc, unsigned char* pixels, int w, int h);
sst_ErrorCode sst_vram_get_location(sst_VRAM* vram, int w, int h, sst_VRAM_Location* loc);
sst_ErrorCode sst_vram_clear(sst_VRAM* vram);

#endif