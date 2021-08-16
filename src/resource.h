#ifndef SST_RESOURCE_H
#define SST_RESOURCE_H

#include "error.h"

typedef enum {
  sst_Res_Prog_Fbo_Fragment,
  sst_Res_Prog_Fbo_Vertex,
  sst_Res_Prog_Sprite_Fragment,
  sst_Res_Prog_Sprite_Vertex,
  sst_Res_Prog_Tile_Fragment,
  sst_Res_Prog_Tile_Vertex,
  sst_Res_Script_Bootstrap,
  sst_Res_Script_Gfx,
  sst_Res_Script_Input,
  sst_Res_Script_Io,
  sst_Res_Script_Json,
  sst_Res_Max
} sst_Resource;

sst_ErrorCode sst_resource_init();
const char* sst_resource_get(sst_Resource id);

#endif