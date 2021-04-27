#ifndef input_wren_h
#define input_wren_h

#include <wren.h>

#include "error.h"
#include "sst_wren.h"

sst_ErrorCode sst_input_wren_register(WrenVM* vm);

#endif

