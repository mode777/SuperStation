#ifndef IO_H
#define IO_H

#include <stdlib.h>

#include "error.h"

sst_ErrorCode sst_io_readfile(const char* path, const char** out_str, size_t* out_size);
const char* sst_io_joinPath(int fragments, ...);

#endif