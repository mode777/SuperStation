#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <GLES2/gl2.h>
#include <wren.h>

#include "sst_common.h"
#include "gfx.h"
#include "image.h"
#include "error.h"
#include "io.h"
#include "quad.h"
#include "sst_wren.h"
#include "sst_zip.h"

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#include "web_api.h"
#endif

static sst_State State; 
static SDL_Event event;
static SDL_Window* window;
static bool quit;

static void load_game(const char* str){
  if(sst_zip_isArchive(str)){
    State.root = str;
    State.isZip = true;
  } else {
    int len = strlen(str);
    if(str[len-1] != '/' && str[len-1] != '\\'){
      State.root = sst_io_joinPath(2, str, "/");
    } else {
      State.root = str;
    }
  }
  // TODO: Reset VRAM
  SST_CALL_TERM(sst_sprites_reset(&State.gfx.sprites));
  SST_CALL_TERM(sst_layers_reset(&State.gfx.layers));
  SST_CALL_TERM(sst_wren_dispose_vm(State.vm));
  SST_CALL_TERM(sst_wren_new_vm(&State, &State.vm));
}

static void update(){
    while(SDL_PollEvent(&event)){
      switch (event.type)
      {
        case SDL_QUIT:
          quit = true;
          break;
        case SDL_DROPFILE:
          load_game(event.drop.file);      
        default:
          break;
      }
    }

    SST_CALL_TERM(sst_gfx_update(&State.gfx));
    SST_CALL_TERM(sst_gfx_draw(&State.gfx));
    bool running;
    if(State.vm != NULL){
      SST_CALL_TERM(sst_wren_update(State.vm, &running));
    }
    //quit = quit ? quit : !running;

    SDL_GL_SwapWindow(window);

    if(glGetError() != GL_NO_ERROR){
      sst_error_add("OpengGL reported an error");
      SST_TERM();
    }
}

int main(int argc, char *argv[]) {

  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);

  window = SDL_CreateWindow("SuperStation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SST_WIN_WIDTH, SST_WIN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  assert(window != NULL);

  SDL_GLContext* context = SDL_GL_CreateContext(window);

  assert(context != NULL);

  SDL_GL_SetSwapInterval(1);
  SST_CALL_TERM(sst_gfx_init(&State.gfx));

  if(argc > 1){    
    load_game(argv[1]);
  }

#ifdef EMSCRIPTEN
  SST_CALL_TERM(sst_web_init(&State))
  emscripten_set_main_loop(update, 0, 1);
#else
  while(!quit){
    update();
  }
#endif
}