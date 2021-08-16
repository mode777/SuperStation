#include "resource.h"
#include <stddef.h>

#ifdef SST_INCLUDE_ASSETS
#include "resources.in"
#else
#include "io.h"
#endif

static const char* resources[sst_Res_Max];

#ifdef SST_INCLUDE_ASSETS
sst_ErrorCode sst_resource_init() {
  resources[sst_Res_Prog_Fbo_Fragment] = RES_PROG_FBO_FRAGMENT;
  resources[sst_Res_Prog_Fbo_Vertex] = RES_PROG_FBO_VERTEX;
  resources[sst_Res_Prog_Sprite_Fragment] = RES_PROG_SPRITE_FRAGMENT;
  resources[sst_Res_Prog_Sprite_Vertex] = RES_PROG_SPRITE_VERTEX;
  resources[sst_Res_Prog_Tile_Fragment] = RES_PROG_TILE_FRAGMENT;
  resources[sst_Res_Prog_Tile_Vertex] = RES_PROG_TILE_VERTEX;
  resources[sst_Res_Script_Bootstrap] = RES_SCRIPT_BOOTSTRAP;
  resources[sst_Res_Script_Gfx] = RES_SCRIPT_GFX;
  resources[sst_Res_Script_Input] = RES_SCRIPT_INPUT;
  resources[sst_Res_Script_Io] = RES_SCRIPT_IO;
  resources[sst_Res_Script_Json] = RES_SCRIPT_JSON;
  SST_RETURN();
}
#else
sst_ErrorCode sst_resource_init() {
  SST_TRY_CALL(sst_io_readfile("glsl/fbo.fragment.glsl", &resources[sst_Res_Prog_Fbo_Fragment], NULL));
  SST_TRY_CALL(sst_io_readfile("glsl/fbo.vertex.glsl", &resources[sst_Res_Prog_Fbo_Vertex], NULL));
  SST_TRY_CALL(sst_io_readfile("glsl/sprite.fragment.glsl", &resources[sst_Res_Prog_Sprite_Fragment], NULL));
  SST_TRY_CALL(sst_io_readfile("glsl/sprite.vertex.glsl", &resources[sst_Res_Prog_Sprite_Vertex], NULL));
  SST_TRY_CALL(sst_io_readfile("glsl/tile.fragment.glsl", &resources[sst_Res_Prog_Tile_Fragment], NULL));
  SST_TRY_CALL(sst_io_readfile("glsl/tile.vertex.glsl", &resources[sst_Res_Prog_Tile_Vertex], NULL));
  SST_TRY_CALL(sst_io_readfile("wren/bootstrap.wren", &resources[sst_Res_Script_Bootstrap], NULL));
  SST_TRY_CALL(sst_io_readfile("wren/gfx.wren", &resources[sst_Res_Script_Gfx], NULL));
  SST_TRY_CALL(sst_io_readfile("wren/input.wren", &resources[sst_Res_Script_Input], NULL));
  SST_TRY_CALL(sst_io_readfile("wren/io.wren", &resources[sst_Res_Script_Io], NULL));
  SST_TRY_CALL(sst_io_readfile("wren/json.wren", &resources[sst_Res_Script_Json], NULL));
  SST_RETURN();
}
#endif

const char* sst_resource_get(sst_Resource id) {
  return resources[id];
}