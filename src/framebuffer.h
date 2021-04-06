#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "gfx_common.h"
#include "error.h"

typedef struct {
  GLuint framebuffer;
  sst_Program program;
  GLuint buffer;
  GLuint elementBuffer;
  GLuint texture;
  GLsizei texWidth;
  GLsizei texHeight;
} sst_Framebuffer;

sst_ErrorCode sst_framebuffer_init(sst_Framebuffer* fb);
sst_ErrorCode sst_framebuffer_draw(sst_Framebuffer* fb);

#endif