#ifndef IMAGE_H
#define IMAGE_H

#include <stddef.h>
#include <stdlib.h>

#include "error.h"

sst_ErrorCode sst_image_decode(unsigned char* data, size_t size, unsigned char** out_pixels, size_t* out_w, size_t* out_h);
sst_ErrorCode sst_image_info(unsigned char* data, size_t size, size_t* out_w, size_t* out_h);

#endif