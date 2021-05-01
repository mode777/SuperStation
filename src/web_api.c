//#define EMSCRIPTEN
#ifdef EMSCRIPTEN

#include <stdio.h>

#include "web_api.h"
#include "sst_wren.h"
#include "error.h"
#include "gl.h"
#include "system.h"

sst_State* web_state;

sst_ErrorCode sst_web_init(sst_State* state){
  web_state = state; 
  return sst_NoError;
}

void sst_web_vm_set_paused(bool isPaused){
  sst_wren_set_paused(web_state->vm, isPaused);
}

void* sst_web_vram_read_pixel(int x, int y, int w, int h){
  void* pixel = NULL;
  sst_ErrorCode error = sst_gl_read_pixel(web_state->gfx.vram.texture, x, y, w, h, (char**)&pixel);
  
  if(error != sst_NoError){
    puts(sst_error_get());
  }

  return pixel;
}

void sst_web_vm_interpret(const char* str){
  sst_ErrorCode error = sst_wren_repl(web_state->vm, str);
  
  if(error != sst_NoError){
    puts(sst_error_get());
  }
}

void sst_web_system_loadGame(const char* str){
  sst_ErrorCode error = sst_system_loadGame(web_state, str);
  if(error != sst_NoError){
    puts(sst_error_get());
  }
}

#endif

