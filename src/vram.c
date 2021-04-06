#include "vram.h"
#include "gl.h"

sst_ErrorCode sst_vram_init(sst_VRAM* vram){
  SST_TRY_CALL(sst_gl_create_texture(SST_VRAM_WIDTH, SST_VRAM_HEIGHT, &vram->texture));
  SST_RETURN();
}

sst_ErrorCode sst_vram_upload_image(sst_VRAM* vram, int x, int y, unsigned char* pixels, int w, int h){
  glBindTexture(GL_TEXTURE_2D, vram->texture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)pixels);
  SST_RETURN();
}