#ifndef GL_H
#define GL_H

#include <GLES2/gl2.h>
#include "error.h"

sst_ErrorCode sst_gl_create_program(const char* vertex_src, const char* fragment_src, GLuint* out_program);
sst_ErrorCode sst_gl_create_texture(int w, int h, GLuint* out_texture);
sst_ErrorCode sst_gl_create_framebuffer(int w, int h, GLuint* out_fb, GLuint* out_tex, GLsizei* out_tw, GLsizei* out_th);
sst_ErrorCode sst_gl_read_pixel(GLuint texture, int x, int y, int w, int h, char** out_pixels);

#endif