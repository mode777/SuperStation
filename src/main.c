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
#include "system.h"

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
      switch (event.type)
      {
        case SDL_QUIT:
          quit = true;
          break;
        case SDL_DROPFILE:
          SST_CALL_TERM(sst_system_loadGame(&State, event.drop.file));   
          break;   
        case SDL_KEYUP:
          if(event.key.keysym.scancode == SDL_SCANCODE_F5 && State.root != NULL) {
            SST_CALL_TERM(sst_system_loadGame(&State, State.root));
          } else if(event.key.keysym.scancode == SDL_SCANCODE_1){
            SDL_SetWindowSize(window, SST_FB_WIDTH, SST_FB_HEIGHT);
          } else if(event.key.keysym.scancode == SDL_SCANCODE_2){
            SDL_SetWindowSize(window, SST_FB_WIDTH*2, SST_FB_HEIGHT*2);
          } else if(event.key.keysym.scancode == SDL_SCANCODE_3){
            SDL_SetWindowSize(window, SST_FB_WIDTH*3, SST_FB_HEIGHT*3);
          } else if(event.key.keysym.scancode == SDL_SCANCODE_4){
            SDL_SetWindowSize(window, SST_FB_WIDTH*4, SST_FB_HEIGHT*4);
          }
        default:
          break;
      }
    }
    sst_input_update(&State.input);
    SST_CALL_TERM(sst_gfx_update(&State.gfx));

    unsigned int w,h;
    SDL_GetWindowSize(window, &w, &h);

    SST_CALL_TERM(sst_gfx_draw(&State.gfx, w, h));
    bool running;
    if(State.vm != NULL){
      sst_ErrorCode error = sst_wren_update(State.vm, &running);
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

  window = SDL_CreateWindow("SuperStation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SST_WIN_WIDTH, SST_WIN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  assert(window != NULL);

  SDL_GLContext* context = SDL_GL_CreateContext(window);

  assert(context != NULL);

  SDL_GL_SetSwapInterval(1);
  SST_CALL_TERM(sst_gfx_init(&State.gfx));

  if(argc > 1){    
    SST_CALL_TERM(sst_system_loadGame(&State, argv[1]));
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