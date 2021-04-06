#ifndef QUAD_H
#define QUAD_H

#include <stddef.h>

#include <GLES2/gl2.h>

#include "gfx_common.h"

typedef struct {
  GLshort x;
  GLshort y;
  GLushort u;
  GLushort v;
  GLfloat sx;
  GLfloat sy;
  GLshort r;
  GLubyte prio;
  GLubyte intensity;
  GLshort tx;
  GLshort ty;
  sst_Color color;
  GLubyte opacity;
} sst_Vertex;

typedef struct {
  sst_Vertex corners[4];
} sst_Quad;

void sst_quad_reset(sst_Quad* q);
void sst_quad_set_shape(sst_Quad* q, int x, int y, int w, int h, int ox, int oy);
void sst_quad_set_source(sst_Quad* q, short x, short y, short w, short h);
void sst_quad_set_translation(sst_Quad* q, short x, short y);
void sst_quad_offset_translation(sst_Quad* q, short x, short y);
void sst_quad_set_rotation(sst_Quad* q, double r);
void sst_quad_offset_rotation(sst_Quad* q, double r);
void sst_quad_set_scale(sst_Quad* q, double sx, double sy);
void sst_quad_offset_scale(sst_Quad* q, double sx, double sy);
void sst_quad_set_layer(sst_Quad* q, char layer);
void sst_quad_set_color(sst_Quad* q, char r, char g, char b);
void sst_quad_set_opacity(sst_Quad* q, char opacity);
void sst_quad_set_intensity(sst_Quad* q, char intensity);

#endif