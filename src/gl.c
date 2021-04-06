#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include <GLES2/gl2.h>

#include "gl.h"
#include "error.h"

static sst_ErrorCode createShader(GLenum type, const char* src, GLuint* out_shader){
  GLuint shader = glCreateShader(type);
  size_t vlength = strlen(src);
  GLint len = vlength;

  glShaderSource(shader, 1, &src, &len);
  glCompileShader(shader);
  
  int success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(!success){
    char buf[512];
    GLsizei size;
    glGetShaderInfoLog(shader, sizeof(buf), &size, buf);
    if(type == GL_VERTEX_SHADER){
      sst_error_add("Vertex shader compilation error:");
    } else {
      sst_error_add("Fragment shader compilation error:");
    }
    sst_error_add(buf);
    glDeleteShader(shader);
    return sst_Error;
  }
  SST_RETURN(*out_shader = shader);
}

sst_ErrorCode sst_gl_create_program(const char* vertex_src, const char* fragment_src, GLuint* out_program){
  GLuint vshader, fshader, program;
  
  SST_TRY_CALL(createShader(GL_VERTEX_SHADER, vertex_src, &vshader));
  SST_TRY_CALL(createShader(GL_FRAGMENT_SHADER, fragment_src, &fshader));

  program = glCreateProgram();
  glAttachShader(program, vshader);
  glAttachShader(program, fshader);
  glLinkProgram(program);

  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if(!success){
    char buf[512];
    GLsizei size;
    glGetProgramInfoLog(program, sizeof(buf), &size, buf);
    sst_error_add("Program link error:");
    sst_error_add(buf);
    glDeleteProgram(program);
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    return sst_Error;
  }
  
  glDeleteShader(vshader);
  glDeleteShader(fshader);
  SST_RETURN(*out_program = program);
}


sst_ErrorCode sst_gl_create_texture(int w, int h, GLuint* out_texture){
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  SST_RETURN(*out_texture = tex);
}

// Next power of two
#define SST_NEXTPOT(V) 1 << (int)ceil(log2(V))

sst_ErrorCode sst_gl_create_framebuffer(int w, int h, GLuint* out_fb, GLuint* out_tex, GLsizei* out_tw, GLsizei* out_th){
  GLuint tex, fb;  
  int tw = SST_NEXTPOT(w), th = SST_NEXTPOT(h);
  SST_TRY_CALL(sst_gl_create_texture(tw, th, &tex));
  glGenFramebuffers(1, &fb);
  glBindFramebuffer(GL_FRAMEBUFFER, fb);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  if(status != GL_FRAMEBUFFER_COMPLETE){
    glDeleteTextures(1, &tex);
    glDeleteFramebuffers(1, &fb);
    SST_RETURN_ERROR("Framebuffer incomplete");
  }
  SST_RETURN(*out_fb = fb;*out_tex = tex; *out_tw=tw; *out_th=th);
}

sst_ErrorCode sst_gl_read_pixel(GLuint texture, int x, int y, int w, int h, char** out_pixels){
  GLuint fbo;
  glBindTexture(GL_TEXTURE_2D, texture);
  char* pixels = calloc(w*h*4, sizeof(char));
  glGenFramebuffers(1, &fbo); 
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

  glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if(status != GL_FRAMEBUFFER_COMPLETE) printf("Framebuffer incomplete\n");

  sst_ErrorCode error = sst_NoError;
  GLenum glError = glGetError();
  
  if(glError != GL_NO_ERROR){
    printf("GLERROR: %i\n", glError);
    sst_error_add("OpenGL error");
    error = sst_Error;
    free(pixels);
  } else {
    *out_pixels = pixels;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDeleteFramebuffers(1, &fbo);

  return error;
}