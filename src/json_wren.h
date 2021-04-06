#ifndef json_wren_h
#define json_wren_h

#include <wren.h>

#include "error.h"
#include "sst_wren.h"

sst_ErrorCode sst_json_wren_register(WrenVM* vm);

#endif

