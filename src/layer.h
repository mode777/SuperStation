#ifndef LAYER_H
#define LAYER_H

#include <GLES2/gl2.h>

#include "error.h"
#include "quad.h"

#define SST_MAX_LAYERS 8

typedef struct {
  GLint offsetX;
  GLint offsetY;
  GLint width;
  GLint height;
  GLint tileOffsetX;
  GLint tileOffsetY;
  GLint tileW;
  GLint tileH;
  bool enabled;
} sst_LayerAttribs;

typedef struct {
  sst_Quad quads[SST_MAX_LAYERS];
  sst_LayerAttribs attributes[SST_MAX_LAYERS];
  sst_Program program;
  GLuint buffer;
  GLuint indexBuffer; 
} sst_Layers;

sst_ErrorCode sst_layers_init(sst_Layers* sprites);
sst_ErrorCode sst_layers_update(sst_Layers* sprites);
sst_ErrorCode sst_layers_draw(sst_Layers* sprites);

#endif