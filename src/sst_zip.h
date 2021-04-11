#ifndef SST_ZIP_H
#define SST_ZIP_H

#include <stdlib.h>
#include <stdbool.h>

#include "error.h"

sst_ErrorCode sst_zip_readfile(const char* zipPath, const char* path, const char** out_str, size_t* out_size);
bool sst_zip_isArchive(const char* zipPath);

#endif