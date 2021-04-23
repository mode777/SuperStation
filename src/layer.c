#include <stdbool.h>

#include <SDL.h>

#include "io.h"
#include "layer.h"
#include "quad.h"
#include "gl.h"

sst_ErrorCode sst_layers_reset(sst_Layers* layers){
  
  sst_LayerAttribs defaultAttribs = {0};
  // Init vertex attributes
  for (size_t i = 0; i < SST_MAX_LAYERS; i++)
  {

    *(&layers->attributes[i]) = defaultAttribs;

    sst_Quad* q = &layers->quads[i];
    sst_quad_reset(q);
    sst_quad_set_shape(q, 0, 0, 2, 2, 1, 1);
    sst_quad_set_source(q, 0, 0, 1,1);
    sst_quad_set_layer(q, i+1);
  }
  SST_RETURN();
}

sst_ErrorCode sst_layers_init(sst_Layers* layers){
  // Create shaders
  size_t str_size;
  sst_Program* p = &layers->program;
  unsigned char *vertex, *fragment;
  SST_TRY_CALL(sst_io_readfile("glsl/vertex_tile.glsl", &vertex, &str_size));
  SST_TRY_CALL(sst_io_readfile("glsl/fragment_tile.glsl", &fragment, &str_size));
  SST_TRY_CALL(sst_gl_create_program((const char*)vertex, (const char*)fragment, &p->program));
  free((void*)vertex);
  free((void*)fragment);
  
  p->attributes.coordUv = glGetAttribLocation(p->program, "coordUv");
  p->attributes.scale = glGetAttribLocation(p->program, "scale");
  p->attributes.trans = glGetAttribLocation(p->program, "trans");
  p->attributes.rot = glGetAttribLocation(p->program, "rot");
  p->attributes.prioInt = glGetAttribLocation(p->program, "prioIntensity");
  p->attributes.color = glGetAttribLocation(p->program, "color");

  p->uniforms.size = glGetUniformLocation(p->program, "size");
  p->uniforms.texSize = glGetUniformLocation(p->program, "texSize");
  p->uniforms.tilesize = glGetUniformLocation(p->program, "tilesize");
  p->uniforms.texture = glGetUniformLocation(p->program, "texture");
  p->uniforms.prio = glGetUniformLocation(p->program, "prio");
  p->uniforms.mapOffset = glGetUniformLocation(p->program, "mapOffset");
  p->uniforms.mapSize = glGetUniformLocation(p->program, "mapSize");
  p->uniforms.tileOffset = glGetUniformLocation(p->program, "tileOffset");

  // Create buffers
  GLuint buffers[2];
  glGenBuffers(2, buffers);
  layers->buffer = buffers[0];
  layers->indexBuffer = buffers[1];
  glBindBuffer(GL_ARRAY_BUFFER, layers->buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(sst_Quad)*SST_MAX_LAYERS, NULL, GL_STREAM_DRAW);

  // Write indices
  GLushort indices[SST_MAX_LAYERS*6];
  for (size_t i = 0; i < SST_MAX_LAYERS; i++)
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
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, layers->indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), (const GLvoid*)&indices, GL_STATIC_DRAW);

  SST_RETURN();
}

sst_ErrorCode sst_layers_update(sst_Layers* layers){
  glBindBuffer(GL_ARRAY_BUFFER, layers->buffer);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(layers->quads), (const GLvoid*)&layers->quads);
  SST_RETURN();
}

sst_ErrorCode sst_layers_draw(sst_Layers* layers){
  glBindBuffer(GL_ARRAY_BUFFER, layers->buffer);
  sst_AttributeLocations l = layers->program.attributes;
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
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, layers->indexBuffer);
  glDrawElements(GL_TRIANGLES, SST_MAX_LAYERS*6, GL_UNSIGNED_SHORT, NULL);
  SST_RETURN();
}