#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#include <SDL.h>

#include "sprites.h"
#include "io.h"
#include "gl.h"

sst_ErrorCode sst_sprites_reset(sst_Sprites* sprites){
// Init vertex attributes
  for (size_t i = 0; i < SST_MAX_SPRITES; i++)
  {
    sst_quad_reset(&sprites->quads[i]);
  }
  SST_RETURN();
}

sst_ErrorCode sst_sprites_init(sst_Sprites* sprites){
  // Create shaders
  size_t str_size;
  sst_Program* p = &sprites->program;
  const char *vertex, *fragment;
  SST_TRY_CALL(sst_io_readfile("glsl/vertex.glsl", &vertex, &str_size));
  SST_TRY_CALL(sst_io_readfile("glsl/fragment.glsl", &fragment, &str_size));
  SST_TRY_CALL(sst_gl_create_program(vertex, fragment, &p->program));
  SDL_free((void*)vertex);
  SDL_free((void*)fragment);
  
  p->attributes.coordUv = glGetAttribLocation(p->program, "coordUv");
  p->attributes.scale = glGetAttribLocation(p->program, "scale");
  p->attributes.trans = glGetAttribLocation(p->program, "trans");
  p->attributes.rot = glGetAttribLocation(p->program, "rot");
  p->attributes.prioInt = glGetAttribLocation(p->program, "prioIntensity");
  p->attributes.color = glGetAttribLocation(p->program, "color");

  p->uniforms.size = glGetUniformLocation(p->program, "size");
  p->uniforms.texSize = glGetUniformLocation(p->program, "texSize");
  p->uniforms.texture = glGetUniformLocation(p->program, "texture");
  p->uniforms.prio = glGetUniformLocation(p->program, "prio");

  // Create buffers
  GLuint buffers[2];
  glGenBuffers(2, buffers);
  sprites->buffer = buffers[0];
  sprites->indexBuffer = buffers[1];
  glBindBuffer(GL_ARRAY_BUFFER, sprites->buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(sst_Quad)*SST_MAX_SPRITES, NULL, GL_STREAM_DRAW);

  // Write indices
  GLushort indices[SST_MAX_SPRITES*6];
  for (size_t i = 0; i < SST_MAX_SPRITES; i++)
  {
    GLuint index = i*4;
    GLuint offset = i*6;
    indices[offset] = index+3;
    indices[offset+1] = index+2;
    indices[offset+2] = index+1;
    indices[offset+3] = index+3;
    indices[offset+4] = index+1;
    indices[offset+5] = index+0;
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprites->indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), (const GLvoid*)&indices, GL_STATIC_DRAW);

  SST_RETURN();
}

sst_ErrorCode sst_sprites_update(sst_Sprites* sprites){
  glBindBuffer(GL_ARRAY_BUFFER, sprites->buffer);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sprites->quads), (const GLvoid*)&sprites->quads);
  SST_RETURN();
}

sst_ErrorCode sst_sprites_draw(sst_Sprites* sprites){
  glBindBuffer(GL_ARRAY_BUFFER, sprites->buffer);
  sst_AttributeLocations l = sprites->program.attributes;
  glVertexAttribPointer(l.coordUv, 4, GL_SHORT, false, sizeof(sst_Vertex), (const GLvoid*)offsetof(sst_Vertex, x));
  glVertexAttribPointer(l.scale, 2, GL_FLOAT, false, sizeof(sst_Vertex), (const GLvoid*)offsetof(sst_Vertex, sx));
  glVertexAttribPointer(l.trans, 2, GL_SHORT, false, sizeof(sst_Vertex), (const GLvoid*)offsetof(sst_Vertex, tx));
  glVertexAttribPointer(l.rot, 1, GL_UNSIGNED_SHORT, false, sizeof(sst_Vertex), (const GLvoid*)offsetof(sst_Vertex, r));
  glVertexAttribPointer(l.prioInt, 2, GL_UNSIGNED_BYTE, false, sizeof(sst_Vertex), (const GLvoid*)offsetof(sst_Vertex, prio));
  glVertexAttribPointer(l.color, 4, GL_UNSIGNED_BYTE, true, sizeof(sst_Vertex), (const GLvoid*)offsetof(sst_Vertex, color));
  glEnableVertexAttribArray(l.coordUv);
  glEnableVertexAttribArray(l.scale);
  glEnableVertexAttribArray(l.trans);
  glEnableVertexAttribArray(l.rot);
  glEnableVertexAttribArray(l.prioInt);
  glEnableVertexAttribArray(l.color);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprites->indexBuffer);
  glDrawElements(GL_TRIANGLES, SST_MAX_SPRITES*6, GL_UNSIGNED_SHORT, NULL);
  SST_RETURN();
}