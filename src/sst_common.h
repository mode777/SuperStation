#ifndef SST_COMMON_H
#define SST_COMMON_H

#include <string.h>
#include <stdbool.h>

#include <wren.h>

#include "gfx.h"
#include "input.h"

typedef struct {
  bool noGame;
  bool isZip;
  const char* root;
} sst_GameInfo;

typedef struct {
  sst_Gfx gfx;
  sst_InputState input;
  sst_GameInfo game;
  WrenVM* vm;
} sst_State;

const char* sst_string_join(const char* a, const char* b);

#endif