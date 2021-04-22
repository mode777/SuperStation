#ifndef GFX_COMMON_H
#define GFX_COMMON_H

#include <GLES2/gl2.h>

#define SST_MAX_LAYERS 8

#define SST_FB_WIDTH 320
#define SST_FB_HEIGHT 240

#define SST_WIN_WIDTH 1280
#define SST_WIN_HEIGHT 920

typedef struct {
  GLubyte r;
  GLubyte g;
  GLubyte b;
} sst_Color;

typedef struct {
  GLubyte r;
  GLubyte g;
  GLubyte b;
  GLubyte a;
} sst_ColorAlpha;

typedef struct {
  GLuint coordUv;
  GLuint scale;
  GLuint trans;
  GLuint rot;
  GLuint prioInt;
  GLuint color;
} sst_AttributeLocations;

typedef struct {
  GLuint size;
  GLuint texSize;
  GLuint texture;
  GLuint prio;
  GLuint tilesize;
  GLuint mapSize;
  GLuint mapOffset;
  GLuint tileOffset;
} sst_UniformLocations;

typedef struct {
  GLuint program;
  sst_AttributeLocations attributes;
  sst_UniformLocations uniforms;
} sst_Program;

#endif