#ifndef SST_COMMON_H
#define SST_COMMON_H

#include <string.h>
#include <stdbool.h>

#include <wren.h>

#include "gfx.h"
#include "input.h"

typedef struct {
  sst_Gfx gfx;
  sst_InputState input;
  bool isZip;
  const char* root;
  WrenVM* vm;
} sst_State;

const char* sst_string_join(const char* a, const char* b);

#endif