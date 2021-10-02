#include "io_wren.h"

static void io_file_read_1(WrenVM* vm){
    sst_InputState* state = &sst_wren_get_state(vm)->input;
    const char* file = wrenGetSlotString(vm, 1);
    unsigned char* content;
    sst_ErrorCode error = sst_wren_load_resource(vm, file, (unsigned char**)&content, NULL);
  
    if(error != sst_NoError){
        wrenError(vm, sst_error_get());
        sst_error_clear();
        return;
    }

    wrenSetSlotString(vm, 0, (const char *)content);    
    free(content);
}

sst_ErrorCode sst_io_wren_register(WrenVM* vm){
  sst_wren_register_method(vm, "io.File.load(_)", io_file_read_1);
  return sst_NoError;
}