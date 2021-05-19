#include "vram.h"
#include "gl.h"

sst_ErrorCode sst_vram_init(sst_VRAM* vram){
  SST_TRY_CALL(sst_gl_create_texture(SST_VRAM_WIDTH, SST_VRAM_HEIGHT, &vram->texture));
  SST_RETURN();
}

sst_ErrorCode sst_vram_upload_image(sst_VRAM* vram, sst_VRAM_Location loc, unsigned char* pixels, int w, int h){
  if(loc.x + w > SST_VRAM_WIDTH || loc.y + h > SST_VRAM_HEIGHT){
    sst_error_add("VRAM upload image out of bounds");
    return sst_Error;
  }
  glBindTexture(GL_TEXTURE_2D, vram->texture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, loc.x, loc.y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)pixels);
  SST_RETURN();
}

sst_ErrorCode sst_vram_clear(sst_VRAM* vram){
  glDeleteTextures(1, &vram->texture);
  SST_TRY_CALL(sst_vram_init(vram));
  return sst_NoError;
}