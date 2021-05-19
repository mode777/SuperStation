#include "system.h"
#include "sst_zip.h"
#include "io.h"
#include "gfx.h"
#include "sst_wren.h"


sst_ErrorCode sst_system_loadGame(sst_State* state, const char* str){
  if(sst_zip_isArchive(str)){
    state->root = str;
    state->isZip = true;
  } else {
    int len = strlen(str);
    if(str[len-1] != '/' && str[len-1] != '\\'){
      state->root = sst_io_joinPath(2, str, "/");
    } else {
      state->root = str;
    }
    state->isZip = false;
  }
  SST_TRY_CALL(sst_vram_clear(&state->gfx.vram));
  SST_TRY_CALL(sst_sprites_reset(&state->gfx.sprites));
  SST_TRY_CALL(sst_layers_reset(&state->gfx.layers));
  SST_TRY_CALL(sst_wren_dispose_vm(state->vm));
  SST_TRY_CALL(sst_wren_new_vm(state, &state->vm));
  
  sst_input_init(&state->input);

  SST_RETURN();
}
