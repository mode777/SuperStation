#ifndef io_wren_h
#define io_wren_h

#include <wren.h>

#include "error.h"
#include "sst_wren.h"

sst_ErrorCode sst_io_wren_register(WrenVM* vm);

#endif

