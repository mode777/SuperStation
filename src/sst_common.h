#ifndef SST_COMMON_H
#define SST_COMMON_H

#include <string.h>
#include <stdbool.h>

#include <wren.h>

#include "gfx.h"

static char* SST_PREFIX_SRC = "/src/";
static char* SST_PREFIX_IMG = "/img/";

typedef struct {
  sst_Gfx gfx;
  bool isZip;
  const char* root;
  WrenVM* vm;
} sst_State;

const char* sst_string_join(const char* a, const char* b);

#endif