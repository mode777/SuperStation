#include <stddef.h>
#include <stdlib.h>

#include "error.h"
#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

sst_ErrorCode sst_image_decode(unsigned char* data, size_t size, unsigned char** out_pixels, size_t* out_w, size_t* out_h){
  int x, y, n;
  unsigned char* pixels = stbi_load_from_memory(data, (int)size, &x, &y, &n, 4);
  if(pixels == NULL){
    SST_RETURN_ERROR(stbi_failure_reason());
  }
  SST_RETURN(*out_pixels = pixels; if(out_w){*out_w = x;} if(out_h){*out_h = y;});
}

sst_ErrorCode sst_image_info(unsigned char* data, size_t size, size_t* out_w, size_t* out_h){
  int x, y, n;
  int error = stbi_info_from_memory(data, size, &x, &y, &n);
  if(error == 0){
    SST_RETURN_ERROR(stbi_failure_reason());
  }
  SST_RETURN(*out_w = x; *out_h = y);
}