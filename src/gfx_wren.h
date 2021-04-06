#ifndef GFX_WREN_H
#define GFX_WREN_H

#include <wren.h>

#include "error.h"
#include "sst_wren.h"

sst_ErrorCode sst_gfx_wren_register(WrenVM* vm);

#endif