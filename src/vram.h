#ifndef VRAM_H
#define VRAM_H

#include "error.h"
#include "gfx_common.h"

#define SST_VRAM_WIDTH 1024
#define SST_VRAM_HEIGHT 1024

typedef struct {
  GLuint texture;
} sst_VRAM;

sst_ErrorCode sst_vram_init(sst_VRAM* vram);
sst_ErrorCode sst_vram_upload_image(sst_VRAM* vram, int x, int y, unsigned char* pixels, int w, int h);

#endif