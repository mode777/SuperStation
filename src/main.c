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
#include "gfx_wren.h"
#include "json_wren.h"

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#include "web_api.h"
#endif

static sst_State State; 
static SDL_Event event;
static SDL_Window* window;
static bool quit;

static void update(){
    while(SDL_PollEvent(&event)){
      if(event.type == SDL_QUIT) quit = true;
    }

    SST_CALL_TERM(sst_gfx_update(&State.gfx));
    SST_CALL_TERM(sst_gfx_draw(&State.gfx));
    bool running;
    SST_CALL_TERM(sst_wren_update(State.vm, &running));
    //quit = quit ? quit : !running;

    SDL_GL_SwapWindow(window);

    if(glGetError() != GL_NO_ERROR){
      sst_error_add("OpengGL reported an error");
      SST_TERM();
    }
}

int main(int argc, char *argv[]) {
  if(argc < 2){
    puts("No Game");
    return 0;
  }

  int len = strlen(argv[1]);
  if(argv[1][len-1] != '/' && argv[1][len-1] != '\\'){
    State.root = calloc(strlen(argv[1]) + 2, sizeof(char));
    strcpy((char*)State.root, argv[1]);
    strcat((char*)State.root, "/");
  } else {
    State.root = argv[0];
  }

  State.isZip = false;

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

  sst_wren_init(&State, &State.vm);
  SST_CALL_TERM(sst_gfx_wren_register(State.vm));
  SST_CALL_TERM(sst_json_wren_register(State.vm));

#ifdef EMSCRIPTEN
  SST_CALL_TERM(sst_web_init(&State))
  emscripten_set_main_loop(update, 0, 1);
#else
  while(!quit){
    update();
  }
#endif
}