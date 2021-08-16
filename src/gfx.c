#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#include <GLES2/gl2.h>
#include <SDL.h>

#include "gfx.h"
#include "gl.h"
#include "error.h"
#include "io.h"
#include "vram.h"
#include "framebuffer.h"

sst_ErrorCode sst_gfx_init(sst_Gfx* state){
  SST_TRY_CALL(sst_sprites_init(&state->sprites));
  SST_TRY_CALL(sst_layers_init(&state->layers));
  SST_TRY_CALL(sst_vram_init(&state->vram));
  SST_TRY_CALL(sst_framebuffer_init(&state->framebuffer));
  glEnable(GL_BLEND);
  return sst_NoError;
}

sst_ErrorCode sst_gfx_update(sst_Gfx* state){
  SST_TRY_CALL(sst_sprites_update(&state->sprites));
  SST_TRY_CALL(sst_layers_update(&state->layers));
  return sst_NoError;
}

sst_ErrorCode sst_gfx_draw(sst_Gfx* state, unsigned int w, unsigned int h){
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindFramebuffer(GL_FRAMEBUFFER, state->framebuffer.framebuffer);

  glViewport(0, 0, SST_FB_WIDTH, SST_FB_HEIGHT);
  glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, state->vram.texture);

  for (size_t i = 0; i < SST_MAX_LAYERS; i++)
  {
    // layers
    sst_Program p = state->layers.program;
    sst_LayerAttribs a = state->layers.attributes[i];
    
    if(a.enabled){      
      //printf("Draw tile layer %i\n", i);
      glUseProgram(p.program);
      glUniform2f(p.uniforms.size, SST_FB_WIDTH, SST_FB_HEIGHT);
      glUniform2f(p.uniforms.texSize, SST_VRAM_WIDTH, SST_VRAM_HEIGHT);
      glUniform2f(p.uniforms.tilesize, a.tileW, a.tileH);
      glUniform1i(p.uniforms.texture, 0);
      glUniform1f(p.uniforms.prio, i+1);
      glUniform2f(p.uniforms.mapOffset, a.offsetX, a.offsetY);
      glUniform2f(p.uniforms.mapSize, a.width, a.height);
      glUniform2f(p.uniforms.tileOffset, a.tileOffsetX, a.tileOffsetY);
      SST_TRY_CALL(sst_layers_draw(&state->layers));
    }

    if(state->sprites.spritesOnLayer[i] > 0){
      //printf("Draw sprite layer %i\n", i);

      // sprites
      p = state->sprites.program;
      glUseProgram(p.program);
      glUniform2f(p.uniforms.size, SST_FB_WIDTH, SST_FB_HEIGHT);
      glUniform2f(p.uniforms.texSize, SST_VRAM_WIDTH, SST_VRAM_HEIGHT);
      glUniform1i(p.uniforms.texture, 0);
      glUniform1f(p.uniforms.prio, i+1);
      SST_TRY_CALL(sst_sprites_draw(&state->sprites));
    }    
  }
  

  SST_TRY_CALL(sst_framebuffer_draw(&state->framebuffer, w, h));
  return sst_NoError;
}



