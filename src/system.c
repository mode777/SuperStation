#include "system.h"
#include "sst_zip.h"
#include "io.h"
#include "gfx.h"
#include "sst_wren.h"


sst_ErrorCode sst_system_loadGame(sst_State* state, const char* str){
    state->game.isZip = false;
    state->game.noGame = false;
    state->game.root = NULL;
  
  if(str == NULL){
    state->game.isZip = true;
    state->game.noGame = true;
  }  else if(sst_zip_isArchive(str)){
    state->game.root = str;
    state->game.isZip = true;
  } else {
    int len = strlen(str);
    if(str[len-1] != '/' && str[len-1] != '\\'){
      state->game.root = sst_io_joinPath(2, str, "/");
    } else {
      state->game.root = str;
    }
  }
  SST_TRY_CALL(sst_vram_clear(&state->gfx.vram));
  SST_TRY_CALL(sst_sprites_reset(&state->gfx.sprites));
  SST_TRY_CALL(sst_layers_reset(&state->gfx.layers));
  SST_TRY_CALL(sst_wren_dispose_vm(state->vm));
  SST_TRY_CALL(sst_wren_new_vm(state, &state->vm));
  
  sst_input_init(&state->input);

  SST_RETURN();
}
