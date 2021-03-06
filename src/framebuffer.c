#include <stdbool.h>
#include <stdio.h>

#include <GLES2/gl2.h>

#include "framebuffer.h"
#include "error.h"
#include "io.h"
#include "gl.h"
#include "resource.h"

sst_ErrorCode sst_framebuffer_init(sst_Framebuffer* fb){
  const char* vertex_fbo = sst_resource_get(sst_Res_Prog_Fbo_Vertex);
  const char* fragment_fbo = sst_resource_get(sst_Res_Prog_Fbo_Fragment);
  SST_TRY_CALL(sst_gl_create_program(vertex_fbo, fragment_fbo, &fb->program.program));
  fb->program.uniforms.texture = glGetUniformLocation(fb->program.program, "texture");
  fb->program.uniforms.scale = glGetUniformLocation(fb->program.program, "scale");
  fb->program.attributes.coordUv = glGetAttribLocation(fb->program.program, "coordUv");  

  SST_TRY_CALL(sst_gl_create_framebuffer(SST_FB_WIDTH, SST_FB_HEIGHT, &fb->framebuffer, &fb->texture, &fb->texWidth, &fb->texHeight));

  glGenBuffers(1, &fb->buffer);
  glBindBuffer(GL_ARRAY_BUFFER, fb->buffer);

  float uvw = SST_FB_WIDTH/(float)fb->texWidth, uvh = SST_FB_HEIGHT/(float)fb->texHeight;

  float pos[] = { 
    -1,  1,  0,    uvh, 
    -1, -1,  0,    0,
     1, -1,  uvw,  0, 
     1,  1,  uvw,  uvh
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(pos), &pos, GL_STATIC_DRAW);

  glGenBuffers(1, &fb->elementBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fb->elementBuffer);
  unsigned short idx[] = { 3,2,1,3,1,0 };
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), &idx, GL_STATIC_DRAW);

  if(glGetError() != GL_NO_ERROR){
    SST_RETURN_ERROR("GL Error");
  }

  return sst_NoError;
}

sst_ErrorCode sst_framebuffer_draw(sst_Framebuffer* fb, unsigned int w, unsigned int h){
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0,0,w,h);
  glClearColor(0,0,0,1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(fb->program.program);

  int pixelscale = SST_MAX(w/SST_FB_WIDTH, h/SST_FB_HEIGHT);
  float targetX = SST_FB_WIDTH * pixelscale, targetY = SST_FB_HEIGHT * pixelscale;

  glUniform2f(fb->program.uniforms.scale, targetX / w, targetY / h);
  glBindBuffer(GL_ARRAY_BUFFER, fb->buffer);
  glVertexAttribPointer(fb->program.attributes.coordUv, 4, GL_FLOAT, false, 0, NULL);
  glEnableVertexAttribArray(fb->program.attributes.coordUv);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, fb->texture);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fb->elementBuffer);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
  return sst_NoError;
}