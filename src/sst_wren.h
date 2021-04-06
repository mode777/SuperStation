#ifndef SST_WREN_H
#define SST_WREN_H

#include <wren.h>

#include "error.h"
#include "sst_common.h"

static inline void wrenError(WrenVM* vm, const char* error){
  wrenSetSlotString(vm, 0, error);
  wrenAbortFiber(vm, 0);
}

sst_ErrorCode sst_wren_init(sst_State* state, WrenVM** out_vm);
sst_ErrorCode sst_wren_update(WrenVM* vm, bool* out_running);
sst_ErrorCode sst_wren_repl(WrenVM* vm, const char* code);
sst_ErrorCode sst_wren_register_method(WrenVM* vm, const char* name, WrenForeignMethodFn func);
sst_ErrorCode sst_wren_register_class(WrenVM* vm, const char* name, WrenForeignMethodFn allocator, WrenFinalizerFn finalizer);
sst_State* sst_wren_get_state(WrenVM* vm);
void sst_wren_set_paused(WrenVM* vm, bool isPaused);

#endif