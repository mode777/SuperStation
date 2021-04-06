#include <string.h>
#include <stdlib.h>

#include "sst_common.h"

const char* sst_string_join(const char* a, const char* b){
  char* str = calloc(strlen(a) + strlen(b) + 1, sizeof(char));
  strcpy(str, a);
  strcat(str, b);
  return (const char*)str;
}