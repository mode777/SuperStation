#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>

#include <GLES2/gl2.h>

#include "quad.h"

void sst_quad_reset(sst_Quad* q){
  sst_Vertex defaults = {0};
  defaults.sx = 1;
  defaults.sy = 1;
  for (size_t j = 0; j < 4; j++)
  {
    q->corners[j] = defaults;
  }
}

void sst_quad_set_shape(sst_Quad* q, int x, int y, int w, int h, int ox, int oy){
  q->corners[0].x = x-ox;
  q->corners[0].y = y+h-oy;

  q->corners[1].x = x-ox;
  q->corners[1].y = y-oy;

  q->corners[2].x = x+w-ox;
  q->corners[2].y = y-oy;

  q->corners[3].x = x+w-ox;
  q->corners[3].y = y+h-oy;
}

void sst_quad_set_source(sst_Quad* q, short x, short y, short w, short h){
  q->corners[0].u = x;
  q->corners[0].v = y+h;

  q->corners[1].u = x;
  q->corners[1].v = y;

  q->corners[2].u = x+w;
  q->corners[2].v = y;

  q->corners[3].u = x+w;
  q->corners[3].v = y+h;
}

void sst_quad_set_translation(sst_Quad* q, short x, short y){
  for (size_t j = 0; j < 4; j++)
  {
    q->corners[j].tx = x;
    q->corners[j].ty = y;
  }  
}

void sst_quad_offset_translation(sst_Quad* q, short x, short y){
  for (size_t j = 0; j < 4; j++)
  {
    q->corners[j].tx += x;
    q->corners[j].ty += y;
  }  
}

#define SST_CIRCLE 6.283185307179586

void sst_quad_set_rotation(sst_Quad* q, double r){
  for (size_t j = 0; j < 4; j++)
  {
    q->corners[j].r = (GLshort)(fmod(r, SST_CIRCLE) * 5215);
  }  
}

void sst_quad_offset_rotation(sst_Quad* q, double r){
  for (size_t j = 0; j < 4; j++)
  {
    q->corners[j].r += (GLshort)(fmod(r, SST_CIRCLE) * 5215);
  }  
}

void sst_quad_set_scale(sst_Quad* q, double sx, double sy){
  for (size_t j = 0; j < 4; j++)
  {
    q->corners[j].sx = (GLfloat)sx;
    q->corners[j].sy = (GLfloat)sy;
  }  
}

void sst_quad_offset_scale(sst_Quad* q, double sx, double sy){
  for (size_t j = 0; j < 4; j++)
  {
    q->corners[j].sx += (GLfloat)sx;
    q->corners[j].sy += (GLfloat)sy;
  }  
}

void sst_quad_set_layer(sst_Quad* q, char layer){
  for (size_t j = 0; j < 4; j++)
  {
    q->corners[j].prio = layer;
  }
}

void sst_quad_set_color(sst_Quad* q, char r, char g, char b){
  sst_Color c = {.r=r, .g=g, .b=b };
  for (size_t j = 0; j < 4; j++)
  {
    q->corners[j].color = c;
  }
}

void sst_quad_set_opacity(sst_Quad* q, char opacity){
  for (size_t j = 0; j < 4; j++)
  {
    q->corners[j].opacity = 255-opacity;
  }
}

void sst_quad_set_intensity(sst_Quad* q, char intensity){
  for (size_t j = 0; j < 4; j++)
  {
    q->corners[j].intensity = intensity;
  }
}